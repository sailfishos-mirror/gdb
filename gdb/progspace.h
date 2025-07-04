/* Program and address space management, for GDB, the GNU debugger.

   Copyright (C) 2009-2025 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#ifndef GDB_PROGSPACE_H
#define GDB_PROGSPACE_H

#include "solib.h"
#include "target.h"
#include "gdb_bfd.h"
#include "registry.h"
#include "gdbsupport/safe-iterator.h"
#include "gdbsupport/intrusive_list.h"
#include "gdbsupport/owning_intrusive_list.h"
#include "gdbsupport/refcounted-object.h"
#include "gdbsupport/gdb_ref_ptr.h"
#include <vector>

struct target_ops;
struct bfd;
struct objfile;
struct inferior;
struct exec;
struct address_space;
struct program_space;
struct solib;

/* An address space.  It is used for comparing if
   pspaces/inferior/threads see the same address space and for
   associating caches to each address space.  */
struct address_space : public refcounted_object
{
  /* Create a new address space object, and add it to the list.  */
  address_space ();
  DISABLE_COPY_AND_ASSIGN (address_space);

  /* Returns the integer address space id of this address space.  */
  int num () const
  {
    return m_num;
  }

  /* Per aspace data-pointers required by other GDB modules.  */
  registry<address_space> registry_fields;

private:
  int m_num;
};

using address_space_ref_ptr
  = gdb::ref_ptr<address_space,
		 refcounted_object_delete_ref_policy<address_space>>;

/* Create a new address space.  */

static inline address_space_ref_ptr
new_address_space ()
{
  return address_space_ref_ptr::new_reference (new address_space);
}

/* A program space represents a symbolic view of an address space.
   Roughly speaking, it holds all the data associated with a
   non-running-yet program (main executable, main symbols), and when
   an inferior is running and is bound to it, includes the list of its
   mapped in shared libraries.

   In the traditional debugging scenario, there's a 1-1 correspondence
   among program spaces, inferiors and address spaces, like so:

     pspace1 (prog1) <--> inf1(pid1) <--> aspace1

   In the case of debugging more than one traditional unix process or
   program, we still have:

     |-----------------+------------+---------|
     | pspace1 (prog1) | inf1(pid1) | aspace1 |
     |----------------------------------------|
     | pspace2 (prog1) | no inf yet | aspace2 |
     |-----------------+------------+---------|
     | pspace3 (prog2) | inf2(pid2) | aspace3 |
     |-----------------+------------+---------|

   In the former example, if inf1 forks (and GDB stays attached to
   both processes), the new child will have its own program and
   address spaces.  Like so:

     |-----------------+------------+---------|
     | pspace1 (prog1) | inf1(pid1) | aspace1 |
     |-----------------+------------+---------|
     | pspace2 (prog1) | inf2(pid2) | aspace2 |
     |-----------------+------------+---------|

   However, had inf1 from the latter case vforked instead, it would
   share the program and address spaces with its parent, until it
   execs or exits, like so:

     |-----------------+------------+---------|
     | pspace1 (prog1) | inf1(pid1) | aspace1 |
     |                 | inf2(pid2) |         |
     |-----------------+------------+---------|

   When the vfork child execs, it is finally given new program and
   address spaces.

     |-----------------+------------+---------|
     | pspace1 (prog1) | inf1(pid1) | aspace1 |
     |-----------------+------------+---------|
     | pspace2 (prog1) | inf2(pid2) | aspace2 |
     |-----------------+------------+---------|

   There are targets where the OS (if any) doesn't provide memory
   management or VM protection, where all inferiors share the same
   address space --- e.g. uClinux.  GDB models this by having all
   inferiors share the same address space, but, giving each its own
   program space, like so:

     |-----------------+------------+---------|
     | pspace1 (prog1) | inf1(pid1) |         |
     |-----------------+------------+         |
     | pspace2 (prog1) | inf2(pid2) | aspace1 |
     |-----------------+------------+         |
     | pspace3 (prog2) | inf3(pid3) |         |
     |-----------------+------------+---------|

   The address space sharing matters for run control and breakpoints
   management.  E.g., did we just hit a known breakpoint that we need
   to step over?  Is this breakpoint a duplicate of this other one, or
   do I need to insert a trap?

   Then, there are targets where all symbols look the same for all
   inferiors, although each has its own address space, as e.g.,
   Ericsson DICOS.  In such case, the model is:

     |---------+------------+---------|
     |         | inf1(pid1) | aspace1 |
     |         +------------+---------|
     | pspace  | inf2(pid2) | aspace2 |
     |         +------------+---------|
     |         | inf3(pid3) | aspace3 |
     |---------+------------+---------|

   Note however, that the DICOS debug API takes care of making GDB
   believe that breakpoints are "global".  That is, although each
   process does have its own private copy of data symbols (just like a
   bunch of forks), to the breakpoints module, all processes share a
   single address space, so all breakpoints set at the same address
   are duplicates of each other, even breakpoints set in the data
   space (e.g., call dummy breakpoints placed on stack).  This allows
   a simplification in the spaces implementation: we avoid caring for
   a many-many links between address and program spaces.  Either
   there's a single address space bound to the program space
   (traditional unix/uClinux), or, in the DICOS case, the address
   space bound to the program space is mostly ignored.  */

/* The program space structure.  */

struct program_space
{
  /* Constructs a new empty program space, binds it to ASPACE, and
     adds it to the program space list.  */
  explicit program_space (address_space_ref_ptr aspace);

  /* Releases a program space, and all its contents (shared libraries,
     objfiles, and any other references to the program space in other
     modules).  It is an internal error to call this when the program
     space is the current program space, since there should always be
     a program space.  */
  ~program_space ();

  using objfiles_iterator
    = reference_to_pointer_iterator<intrusive_list<objfile>::iterator>;
  using objfiles_range = iterator_range<objfiles_iterator>;

  /* Return an iterable object that can be used to iterate over all
     objfiles.  The basic use is in a foreach, like:

     for (objfile *objf : pspace->objfiles ()) { ... }  */
  objfiles_range objfiles ()
  {
    return objfiles_range (objfiles_iterator (m_objfiles_list.begin ()));
  }

  using objfiles_safe_range = basic_safe_range<objfiles_range>;

  /* An iterable object that can be used to iterate over all objfiles.
     The basic use is in a foreach, like:

     for (objfile *objf : pspace->objfiles_safe ()) { ... }

     This variant uses a basic_safe_iterator so that objfiles can be
     deleted during iteration.  */
  objfiles_safe_range objfiles_safe ()
  {
    return objfiles_safe_range
      (objfiles_range (objfiles_iterator (m_objfiles_list.begin ())));
  }

  /* Add OBJFILE to the list of objfiles, putting it just before
     BEFORE.  If BEFORE is nullptr, it will go at the end of the
     list.  */
  void add_objfile (std::unique_ptr<objfile> &&objfile,
		    struct objfile *before);

  /* Remove OBJFILE from the list of objfiles.  */
  void remove_objfile (struct objfile *objfile);

  /* Return true if there is more than one object file loaded; false
     otherwise.  */
  bool multi_objfile_p () const;

  /* Free all the objfiles associated with this program space.  */
  void free_all_objfiles ();

  /* Return the objfile containing ADDRESS, or nullptr if the address
     is outside all objfiles in this progspace.  */
  struct objfile *objfile_for_address (CORE_ADDR address);

  /* Set this program space's solib provider.

     The solib provider must be unset prior to calling this method.  */
  void set_solib_ops (solib_ops_up ops)
  {
    gdb_assert (m_solib_ops == nullptr);
    m_solib_ops = std::move (ops);
  };

  /* Unset and free this program space's solib provider.  */
  void unset_solib_ops ()
  { m_solib_ops = nullptr; }

  /* Unset and return this program space's solib provider.  */
  solib_ops_up release_solib_ops ()
  { return std::move (m_solib_ops); }

  /* Get this program space's solib provider.  */
  const struct solib_ops *solib_ops () const
  { return m_solib_ops.get (); }

  /* Return the list of all the solibs in this program space.  */
  owning_intrusive_list<solib> &solibs ()
  { return m_solib_list; }

  /* Similar to `bfd_get_filename (exec_bfd ())` but in original form given
     by user, without symbolic links and pathname resolved.  It is not nullptr
     iff `exec_bfd ()` is not nullptr.  */
  const char *exec_filename () const
  { return m_exec_filename.get (); }

  void set_exec_filename (gdb::unique_xmalloc_ptr<char> filename)
  { m_exec_filename = std::move (filename); }

  /* Close and clear exec_bfd.  If we end up with no target sections
     to read memory from, this unpushes the exec_ops target.  */
  void exec_close ();

  /* Return the exec BFD for this program space.  */
  bfd *exec_bfd () const
  { return ebfd.get (); }

  /* Set the exec BFD for this program space to ABFD.  */
  void set_exec_bfd (gdb_bfd_ref_ptr &&abfd)
  {
    ebfd = std::move (abfd);
  }

  bfd *core_bfd () const
  { return cbfd.get ();  }

  /* Reset saved solib data at the start of an solib event.  This lets
     us properly collect the data when calling solib_add, so it can then
     later be printed.  */
  void clear_solib_cache ();

  /* Returns true iff there's no inferior bound to this program
     space.  */
  bool empty ();

  /* Remove all target sections owned by OWNER.  */
  void remove_target_sections (target_section_owner owner);

  /* Add the sections array defined by SECTIONS to the
     current set of target sections.  */
  void add_target_sections (target_section_owner owner,
			    const std::vector<target_section> &sections);

  /* Add the sections of OBJFILE to the current set of target
     sections.  They are given OBJFILE as the "owner".  */
  void add_target_sections (struct objfile *objfile);

  /* Clear all target sections from M_TARGET_SECTIONS table.  */
  void clear_target_sections ()
  {
    m_target_sections.clear ();
  }

  /* Return a reference to the M_TARGET_SECTIONS table.  */
  std::vector<target_section> &target_sections ()
  {
    return m_target_sections;
  }

  /* Unique ID number.  */
  int num = 0;

  /* The main executable loaded into this program space.  This is
     managed by the exec target.  */

  /* The BFD handle for the main executable.  */
  gdb_bfd_ref_ptr ebfd;
  /* The last-modified time, from when the exec was brought in.  */
  long ebfd_mtime = 0;

  /* Binary file diddling handle for the core file.  */
  gdb_bfd_ref_ptr cbfd;

  /* The address space attached to this program space.  More than one
     program space may be bound to the same address space.  In the
     traditional unix-like debugging scenario, this will usually
     match the address space bound to the inferior, and is mostly
     used by the breakpoints module for address matches.  If the
     target shares a program space for all inferiors and breakpoints
     are global, then this field is ignored (we don't currently
     support inferiors sharing a program space if the target doesn't
     make breakpoints global).  */
  address_space_ref_ptr aspace;

  /* True if this program space's section offsets don't yet represent
     the final offsets of the "live" address space (that is, the
     section addresses still require the relocation offsets to be
     applied, and hence we can't trust the section addresses for
     anything that pokes at live memory).  E.g., for qOffsets
     targets, or for PIE executables, until we connect and ask the
     target for the final relocation offsets, the symbols we've used
     to set breakpoints point at the wrong addresses.  */
  int executing_startup = 0;

  /* True if no breakpoints should be inserted in this program
     space.  */
  int breakpoints_not_allowed = 0;

  /* The object file that the main symbol table was loaded from
     (e.g. the argument to the "symbol-file" or "file" command).  */
  struct objfile *symfile_object_file = NULL;

  /* Number of calls to solib_add.  */
  unsigned int solib_add_generation = 0;

  /* When an solib is added, it is also added to this vector.  This
     is so we can properly report solib changes to the user.  */
  std::vector<solib *> added_solibs;

  /* When an solib is removed, its name is added to this vector.
     This is so we can properly report solib changes to the user.  */
  std::vector<std::string> deleted_solibs;

  /* Per pspace data-pointers required by other GDB modules.  */
  registry<program_space> registry_fields;

private:
  /* All known objfiles are kept in a linked list.  */
  owning_intrusive_list<objfile> m_objfiles_list;

  /* solib_ops implementation used to provide solibs in this program space.  */
  solib_ops_up m_solib_ops;

  /* List of shared objects mapped into this space.  Managed by
     solib.c.  */
  owning_intrusive_list<solib> m_solib_list;

  /* The set of target sections matching the sections mapped into
     this program space.  Managed by both exec_ops and solib.c.  */
  std::vector<target_section> m_target_sections;

  /* See `exec_filename`.  */
  gdb::unique_xmalloc_ptr<char> m_exec_filename;
};

/* The list of all program spaces.  There's always at least one.  */
extern std::vector<struct program_space *>program_spaces;

/* The current program space.  This is always non-null.  */
extern struct program_space *current_program_space;

/* Initialize progspace-related global state.  */
extern void initialize_progspace ();

/* Copies program space SRC to DEST.  Copies the main executable file,
   and the main symbol file.  Returns DEST.  */
extern struct program_space *clone_program_space (struct program_space *dest,
						struct program_space *src);

/* Sets PSPACE as the current program space.  This is usually used
   instead of set_current_space_and_thread when the current
   thread/inferior is not important for the operations that follow.
   E.g., when accessing the raw symbol tables.  If memory access is
   required, then you should use switch_to_program_space_and_thread.
   Otherwise, it is the caller's responsibility to make sure that the
   currently selected inferior/thread matches the selected program
   space.  */
extern void set_current_program_space (struct program_space *pspace);

/* Save/restore the current program space.  */

class scoped_restore_current_program_space
{
public:
  scoped_restore_current_program_space ()
    : m_saved_pspace (current_program_space)
  {}

  ~scoped_restore_current_program_space ()
  { set_current_program_space (m_saved_pspace); }

  DISABLE_COPY_AND_ASSIGN (scoped_restore_current_program_space);

private:
  program_space *m_saved_pspace;
};

/* Maybe create a new address space object, and add it to the list, or
   return a pointer to an existing address space, in case inferiors
   share an address space.  */
extern address_space_ref_ptr maybe_new_address_space ();

/* Update all program spaces matching to address spaces.  The user may
   have created several program spaces, and loaded executables into
   them before connecting to the target interface that will create the
   inferiors.  All that happens before GDB has a chance to know if the
   inferiors will share an address space or not.  Call this after
   having connected to the target interface and having fetched the
   target description, to fixup the program/address spaces
   mappings.  */
extern void update_address_spaces (void);

#endif /* GDB_PROGSPACE_H */
