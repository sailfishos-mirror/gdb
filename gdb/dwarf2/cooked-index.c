/* DIE indexing 

   Copyright (C) 2022-2024 Free Software Foundation, Inc.

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

#include "dwarf2/cooked-index.h"
#include "dwarf2/index-common.h"
#include "dwarf2/read.h"
#include "dwarf2/stringify.h"
#include "dwarf2/index-cache.h"
#include "cp-support.h"
#include "c-lang.h"
#include "ada-lang.h"
#include "dwarf2/tag.h"
#include "event-top.h"
#include "exceptions.h"
#include "split-name.h"
#include "observable.h"
#include "run-on-main-thread.h"
#include <algorithm>
#include "gdbsupport/gdb-safe-ctype.h"
#include "gdbsupport/selftest.h"
#include "gdbsupport/task-group.h"
#include "gdbsupport/thread-pool.h"
#include <chrono>
#include "cli/cli-cmds.h"

/* We don't want gdb to exit while it is in the process of writing to
   the index cache.  So, all live cooked index vectors are stored
   here, and then these are all waited for before exit proceeds.  */
static gdb::unordered_set<cooked_index *> active_vectors;

/* See cooked-index.h.  */

std::string
to_string (cooked_index_flag flags)
{
  static constexpr cooked_index_flag::string_mapping mapping[] = {
    MAP_ENUM_FLAG (IS_MAIN),
    MAP_ENUM_FLAG (IS_STATIC),
    MAP_ENUM_FLAG (IS_LINKAGE),
    MAP_ENUM_FLAG (IS_TYPE_DECLARATION),
    MAP_ENUM_FLAG (IS_PARENT_DEFERRED),
  };

  return flags.to_string (mapping);
}

/* See cooked-index.h.  */

bool
language_requires_canonicalization (enum language lang)
{
  return (lang == language_ada
	  || lang == language_c
	  || lang == language_cplus);
}

/* Return true if a plain "main" could be the main program for this
   language.  Languages that are known to use some other mechanism are
   excluded here.  */

static bool
language_may_use_plain_main (enum language lang)
{
  /* No need to handle "unknown" here.  */
  return (lang == language_c
	  || lang == language_objc
	  || lang == language_cplus
	  || lang == language_m2
	  || lang == language_asm
	  || lang == language_opencl
	  || lang == language_minimal);
}

/* See cooked-index.h.  */

int
cooked_index_entry::compare (const char *stra, const char *strb,
			     comparison_mode mode)
{
#if defined (__GNUC__) && !defined (__clang__) && __GNUC__ <= 7
  /* Work around error with gcc 7.5.0.  */
  auto munge = [] (char c) -> unsigned char
#else
  auto munge = [] (char c) constexpr -> unsigned char
#endif
    {
      /* Treat '<' as if it ended the string.  This lets something
	 like "func<t>" match "func<t<int>>".  See the "Breakpoints in
	 template functions" section in the manual.  */
      if (c == '<')
	return '\0';
      return TOLOWER ((unsigned char) c);
    };

  unsigned char a = munge (*stra);
  unsigned char b = munge (*strb);

  while (a != '\0' && b != '\0' && a == b)
    {
      a = munge (*++stra);
      b = munge (*++strb);
    }

  if (a == b)
    return 0;

  /* When completing, if STRB ends earlier than STRA, consider them as
     equal.  */
  if (mode == COMPLETE && b == '\0')
    return 0;

  return a < b ? -1 : 1;
}

#if GDB_SELF_TEST

namespace {

void
test_compare ()
{
  /* Convenience aliases.  */
  const auto mode_compare = cooked_index_entry::MATCH;
  const auto mode_sort = cooked_index_entry::SORT;
  const auto mode_complete = cooked_index_entry::COMPLETE;

  SELF_CHECK (cooked_index_entry::compare ("abcd", "abcd",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("abcd", "abcd",
					   mode_complete) == 0);

  SELF_CHECK (cooked_index_entry::compare ("abcd", "ABCDE",
					   mode_compare) < 0);
  SELF_CHECK (cooked_index_entry::compare ("ABCDE", "abcd",
					   mode_compare) > 0);
  SELF_CHECK (cooked_index_entry::compare ("abcd", "ABCDE",
					   mode_complete) < 0);
  SELF_CHECK (cooked_index_entry::compare ("ABCDE", "abcd",
					   mode_complete) == 0);

  SELF_CHECK (cooked_index_entry::compare ("name", "name<>",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<>", "name",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name", "name<>",
					   mode_complete) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<>", "name",
					   mode_complete) == 0);

  SELF_CHECK (cooked_index_entry::compare ("name<arg>", "name<arg>",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg>", "name<ag>",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg>", "name<arg>",
					   mode_complete) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg>", "name<ag>",
					   mode_complete) == 0);

  SELF_CHECK (cooked_index_entry::compare ("name<arg<more>>",
					   "name<arg<more>>",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg>",
					   "name<arg<more>>",
					   mode_compare) == 0);

  SELF_CHECK (cooked_index_entry::compare ("name", "name<arg<more>>",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg<more>>", "name",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg<more>>", "name<arg<",
					   mode_compare) == 0);
  SELF_CHECK (cooked_index_entry::compare ("name<arg<more>>", "name<arg<",
					   mode_complete) == 0);

  SELF_CHECK (cooked_index_entry::compare ("", "abcd", mode_compare) < 0);
  SELF_CHECK (cooked_index_entry::compare ("", "abcd", mode_complete) < 0);
  SELF_CHECK (cooked_index_entry::compare ("abcd", "", mode_compare) > 0);
  SELF_CHECK (cooked_index_entry::compare ("abcd", "", mode_complete) == 0);

  SELF_CHECK (cooked_index_entry::compare ("func", "func<type>",
					   mode_sort) == 0);
  SELF_CHECK (cooked_index_entry::compare ("func<type>", "func1",
					   mode_sort) < 0);
}

} /* anonymous namespace */

#endif /* GDB_SELF_TEST */

/* See cooked-index.h.  */

bool
cooked_index_entry::matches (domain_search_flags kind) const
{
  /* Just reject type declarations.  */
  if ((flags & IS_TYPE_DECLARATION) != 0)
    return false;

  return tag_matches_domain (tag, kind, lang);
}

/* See cooked-index.h.  */

const char *
cooked_index_entry::full_name (struct obstack *storage,
			       cooked_index_full_name_flag name_flags,
			       const char *default_sep) const
{
  const char *local_name = ((name_flags & FOR_MAIN) != 0) ? name : canonical;

  if ((flags & IS_LINKAGE) != 0 || get_parent () == nullptr)
    return local_name;

  const char *sep = default_sep;
  switch (lang)
    {
    case language_cplus:
    case language_rust:
    case language_fortran:
      sep = "::";
      break;

    case language_ada:
      if ((name_flags & FOR_ADA_LINKAGE_NAME) != 0)
	{
	  sep = "__";
	  break;
	}
      [[fallthrough]];
    case language_go:
    case language_d:
      sep = ".";
      break;

    default:
      if (sep == nullptr)
	return local_name;
      break;
    }

  /* The FOR_ADA_LINKAGE_NAME flag should only affect Ada entries, so
     disable it here if we don't need it.  */
  if (lang != language_ada)
    name_flags &= ~FOR_ADA_LINKAGE_NAME;

  get_parent ()->write_scope (storage, sep, name_flags);
  obstack_grow0 (storage, local_name, strlen (local_name));
  return (const char *) obstack_finish (storage);
}

/* See cooked-index.h.  */

void
cooked_index_entry::write_scope (struct obstack *storage,
				 const char *sep,
				 cooked_index_full_name_flag flags) const
{
  if (get_parent () != nullptr)
    get_parent ()->write_scope (storage, sep, flags);
  /* When computing the Ada linkage name, the entry might not have
     been canonicalized yet, so use the 'name'.  */
  const char *local_name = ((flags & (FOR_MAIN | FOR_ADA_LINKAGE_NAME)) != 0
			    ? name
			    : canonical);
  obstack_grow (storage, local_name, strlen (local_name));
  obstack_grow (storage, sep, strlen (sep));
}

/* See cooked-index.h.  */

cooked_index_entry *
cooked_index_shard::create (sect_offset die_offset,
			    enum dwarf_tag tag,
			    cooked_index_flag flags,
			    enum language lang,
			    const char *name,
			    cooked_index_entry_ref parent_entry,
			    dwarf2_per_cu *per_cu)
{
  if (tag == DW_TAG_module || tag == DW_TAG_namespace)
    flags &= ~IS_STATIC;
  else if (lang == language_cplus
	   && (tag == DW_TAG_class_type
	       || tag == DW_TAG_interface_type
	       || tag == DW_TAG_structure_type
	       || tag == DW_TAG_union_type
	       || tag == DW_TAG_enumeration_type
	       || tag == DW_TAG_enumerator))
    flags &= ~IS_STATIC;
  else if (tag_is_type (tag))
    flags |= IS_STATIC;

  return new (&m_storage) cooked_index_entry (die_offset, tag, flags,
					      lang, name, parent_entry,
					      per_cu);
}

/* See cooked-index.h.  */

cooked_index_entry *
cooked_index_shard::add (sect_offset die_offset, enum dwarf_tag tag,
			 cooked_index_flag flags, enum language lang,
			 const char *name, cooked_index_entry_ref parent_entry,
			 dwarf2_per_cu *per_cu)
{
  cooked_index_entry *result = create (die_offset, tag, flags, lang, name,
				       parent_entry, per_cu);
  m_entries.push_back (result);

  /* An explicitly-tagged main program should always override the
     implicit "main" discovery.  */
  if ((flags & IS_MAIN) != 0)
    m_main = result;
  else if ((flags & IS_PARENT_DEFERRED) == 0
	   && parent_entry.resolved == nullptr
	   && m_main == nullptr
	   && language_may_use_plain_main (lang)
	   && strcmp (name, "main") == 0)
    m_main = result;

  return result;
}

/* See cooked-index.h.  */

void
cooked_index_shard::handle_gnat_encoded_entry
     (cooked_index_entry *entry,
      htab_t gnat_entries,
      std::vector<cooked_index_entry *> &new_entries)
{
  /* We decode Ada names in a particular way: operators and wide
     characters are left as-is.  This is done to make name matching a
     bit simpler; and for wide characters, it means the choice of Ada
     source charset does not affect the indexer directly.  */
  std::string canonical = ada_decode (entry->name, false, false, false);
  if (canonical.empty ())
    {
      entry->canonical = entry->name;
      return;
    }
  std::vector<std::string_view> names = split_name (canonical.c_str (),
						    split_style::DOT_STYLE);
  std::string_view tail = names.back ();
  names.pop_back ();

  const cooked_index_entry *parent = nullptr;
  for (const auto &name : names)
    {
      uint32_t hashval = dwarf5_djb_hash (name);
      void **slot = htab_find_slot_with_hash (gnat_entries, &name,
					      hashval, INSERT);
      /* CUs are processed in order, so we only need to check the most
	 recent entry.  */
      cooked_index_entry *last = (cooked_index_entry *) *slot;
      if (last == nullptr || last->per_cu != entry->per_cu)
	{
	  const char *new_name = m_names.insert (name);
	  last = create (entry->die_offset, DW_TAG_module,
			 IS_SYNTHESIZED, language_ada, new_name, parent,
			 entry->per_cu);
	  last->canonical = last->name;
	  new_entries.push_back (last);
	  *slot = last;
	}

      parent = last;
    }

  entry->set_parent (parent);
  entry->canonical = m_names.insert (tail);
}

/* Hash a cooked index entry by name pointer value.

   We can use pointer equality here because names come from .debug_str, which
   will normally be unique-ified by the linker.  Also, duplicates are relatively
   harmless -- they just mean a bit of extra memory is used.  */

struct cooked_index_entry_name_ptr_hash
{
  using is_avalanching = void;

  std::uint64_t operator () (const cooked_index_entry *entry) const noexcept
  {
    return ankerl::unordered_dense::hash<const char *> () (entry->name);
  }
};

/* Compare cooked index entries by name pointer value.  */

struct cooked_index_entry_name_ptr_eq
{
  bool operator () (const cooked_index_entry *a,
		    const cooked_index_entry *b) const noexcept
  {
    return a->name == b->name;
  }
};

/* See cooked-index.h.  */

void
cooked_index_shard::finalize (const parent_map_map *parent_maps)
{
  gdb::unordered_set<const cooked_index_entry *,
		     cooked_index_entry_name_ptr_hash,
		     cooked_index_entry_name_ptr_eq> seen_names;

  auto hash_entry = [] (const void *e)
    {
      const cooked_index_entry *entry = (const cooked_index_entry *) e;
      return dwarf5_djb_hash (entry->canonical);
    };

  auto eq_entry = [] (const void *a, const void *b) -> int
    {
      const cooked_index_entry *ae = (const cooked_index_entry *) a;
      const std::string_view *sv = (const std::string_view *) b;
      return (strlen (ae->canonical) == sv->length ()
	      && strncasecmp (ae->canonical, sv->data (), sv->length ()) == 0);
    };

  htab_up gnat_entries (htab_create_alloc (10, hash_entry, eq_entry,
					   nullptr, xcalloc, xfree));
  std::vector<cooked_index_entry *> new_gnat_entries;

  for (cooked_index_entry *entry : m_entries)
    {
      if ((entry->flags & IS_PARENT_DEFERRED) != 0)
	{
	  const cooked_index_entry *new_parent
	    = parent_maps->find (entry->get_deferred_parent ());
	  entry->resolve_parent (new_parent);
	}

      /* Note that this code must be kept in sync with
	 language_requires_canonicalization.  */
      gdb_assert (entry->canonical == nullptr);
      if ((entry->flags & IS_LINKAGE) != 0)
	entry->canonical = entry->name;
      else if (entry->lang == language_ada)
	{
	  /* Newer versions of GNAT emit DW_TAG_module and use a
	     hierarchical structure.  In this case, we don't need to
	     do any extra work.  This can be detected by looking for a
	     GNAT-encoded name.  */
	  if (strstr (entry->name, "__") == nullptr)
	    {
	      entry->canonical = entry->name;

	      /* If the entry does not have a parent, then there's
		 nothing extra to do here -- the entry itself is
		 sufficient.

		 However, if it does have a parent, we have to
		 synthesize an entry with the full name.  This is
		 unfortunate, but it's necessary due to how some of
		 the Ada name-lookup code currently works.  For
		 example, without this, ada_get_tsd_type will
		 fail.

		 Eventually it would be good to change the Ada lookup
		 code, and then remove these entries (and supporting
		 code in cooked_index_entry::full_name).  */
	      if (entry->get_parent () != nullptr)
		{
		  const char *fullname
		    = entry->full_name (&m_storage, FOR_ADA_LINKAGE_NAME);
		  cooked_index_entry *linkage = create (entry->die_offset,
							entry->tag,
							(entry->flags
							 | IS_LINKAGE
							 | IS_SYNTHESIZED),
							language_ada,
							fullname,
							nullptr,
							entry->per_cu);
		  linkage->canonical = fullname;
		  new_gnat_entries.push_back (linkage);
		}
	    }
	  else
	    handle_gnat_encoded_entry (entry, gnat_entries.get (),
				       new_gnat_entries);
	}
      else if (entry->lang == language_cplus || entry->lang == language_c)
	{
	  auto [it, inserted] = seen_names.insert (entry);

	  if (inserted)
	    {
	      /* No entry with that name was present, compute the canonical
		 name.  */
	      gdb::unique_xmalloc_ptr<char> canon_name
		= (entry->lang == language_cplus
		   ? cp_canonicalize_string (entry->name)
		   : c_canonicalize_name (entry->name));
	      if (canon_name == nullptr)
		entry->canonical = entry->name;
	      else
		entry->canonical = m_names.insert (std::move (canon_name));
	    }
	  else
	    {
	      /* An entry with that name was present, re-use its canonical
		 name.  */
	      entry->canonical = (*it)->canonical;
	    }
	}
      else
	entry->canonical = entry->name;
    }

  /* Make sure any new Ada entries end up in the results.  This isn't
     done when creating these new entries to avoid invalidating the
     m_entries iterator used in the foreach above.  */
  m_entries.insert (m_entries.end (), new_gnat_entries.begin (),
		    new_gnat_entries.end ());

  m_entries.shrink_to_fit ();
  std::sort (m_entries.begin (), m_entries.end (),
	     [] (const cooked_index_entry *a, const cooked_index_entry *b)
	     {
	       return *a < *b;
	     });
}

/* See cooked-index.h.  */

cooked_index_shard::range
cooked_index_shard::find (const std::string &name, bool completing) const
{
  struct comparator
  {
    cooked_index_entry::comparison_mode mode;

    bool operator() (const cooked_index_entry *entry,
		     const char *name) const noexcept
    {
      return cooked_index_entry::compare (entry->canonical, name, mode) < 0;
    }

    bool operator() (const char *name,
		     const cooked_index_entry *entry) const noexcept
    {
      return cooked_index_entry::compare (entry->canonical, name, mode) > 0;
    }
  };

  return std::make_from_tuple<range>
    (std::equal_range (m_entries.cbegin (), m_entries.cend (), name.c_str (),
		       comparator { (completing
				     ? cooked_index_entry::COMPLETE
				     : cooked_index_entry::MATCH) }));
}

/* See cooked-index.h.  */

void
cooked_index_worker::start ()
{
  gdb::thread_pool::g_thread_pool->post_task ([this] ()
  {
    try
      {
	do_reading ();
      }
    catch (const gdb_exception &exc)
      {
	m_failed = exc;
	set (cooked_state::CACHE_DONE);
      }

    bfd_thread_cleanup ();
  });
}

/* See cooked-index.h.  */

bool
cooked_index_worker::wait (cooked_state desired_state, bool allow_quit)
{
  bool done;
#if CXX_STD_THREAD
  {
    std::unique_lock<std::mutex> lock (m_mutex);

    /* This may be called from a non-main thread -- this functionality
       is needed for the index cache -- but in this case we require
       that the desired state already have been attained.  */
    gdb_assert (is_main_thread () || desired_state <= m_state);

    while (desired_state > m_state)
      {
	if (allow_quit)
	  {
	    std::chrono::milliseconds duration { 15 };
	    if (m_cond.wait_for (lock, duration) == std::cv_status::timeout)
	      QUIT;
	  }
	else
	  m_cond.wait (lock);
      }
    done = m_state == cooked_state::CACHE_DONE;
  }
#else
  /* Without threads, all the work is done immediately on the main
     thread, and there is never anything to wait for.  */
  done = desired_state == cooked_state::CACHE_DONE;
#endif /* CXX_STD_THREAD */

  /* Only the main thread is allowed to report complaints and the
     like.  */
  if (!is_main_thread ())
    return false;

  if (m_reported)
    return done;
  m_reported = true;

  /* Emit warnings first, maybe they were emitted before an exception
     (if any) was thrown.  */
  m_warnings.emit ();

  if (m_failed.has_value ())
    {
      /* do_reading failed -- report it.  */
      exception_print (gdb_stderr, *m_failed);
      m_failed.reset ();
      return done;
    }

  /* Only show a given exception a single time.  */
  gdb::unordered_set<gdb_exception> seen_exceptions;
  for (auto &one_result : m_results)
    {
      re_emit_complaints (std::get<1> (one_result));
      for (auto &one_exc : std::get<2> (one_result))
	if (seen_exceptions.insert (one_exc).second)
	  exception_print (gdb_stderr, one_exc);
    }

  print_stats ();

  struct objfile *objfile = m_per_objfile->objfile;
  dwarf2_per_bfd *per_bfd = m_per_objfile->per_bfd;
  cooked_index *table
    = (gdb::checked_static_cast<cooked_index *>
       (per_bfd->index_table.get ()));

  auto_obstack temp_storage;
  enum language lang = language_unknown;
  const char *main_name = table->get_main_name (&temp_storage, &lang);
  if (main_name != nullptr)
    set_objfile_main_name (objfile, main_name, lang);

  /* dwarf_read_debug_printf ("Done building psymtabs of %s", */
  /* 			   objfile_name (objfile)); */

  return done;
}

/* See cooked-index.h.  */

void
cooked_index_worker::set (cooked_state desired_state)
{
  gdb_assert (desired_state != cooked_state::INITIAL);

#if CXX_STD_THREAD
  std::lock_guard<std::mutex> guard (m_mutex);
  gdb_assert (desired_state > m_state);
  m_state = desired_state;
  m_cond.notify_one ();
#else
  /* Without threads, all the work is done immediately on the main
     thread, and there is never anything to do.  */
#endif /* CXX_STD_THREAD */
}

/* See cooked-index.h.  */

void
cooked_index_worker::write_to_cache (const cooked_index *idx,
				     deferred_warnings *warn) const
{
  if (idx != nullptr)
    {
      /* Writing to the index cache may cause a warning to be emitted.
	 See PR symtab/30837.  This arranges to capture all such
	 warnings.  This is safe because we know the deferred_warnings
	 object isn't in use by any other thread at this point.  */
      scoped_restore_warning_hook defer (warn);
      m_cache_store.store ();
    }
}

cooked_index::cooked_index (cooked_index_worker_up &&worker)
  : m_state (std::move (worker))
{
  /* ACTIVE_VECTORS is not locked, and this assert ensures that this
     will be caught if ever moved to the background.  */
  gdb_assert (is_main_thread ());
  active_vectors.insert (this);
}

void
cooked_index::start_reading ()
{
  m_state->start ();
}

void
cooked_index::wait (cooked_state desired_state, bool allow_quit)
{
  gdb_assert (desired_state != cooked_state::INITIAL);

  /* If the state object has been deleted, then that means waiting is
     completely done.  */
  if (m_state == nullptr)
    return;

  if (m_state->wait (desired_state, allow_quit))
    {
      /* Only the main thread can modify this.  */
      gdb_assert (is_main_thread ());
      m_state.reset (nullptr);
    }
}

void
cooked_index::set_contents (std::vector<cooked_index_shard_up> &&shards,
			    deferred_warnings *warn,
			    const parent_map_map *parent_maps)
{
  gdb_assert (m_shards.empty ());
  m_shards = std::move (shards);

  m_state->set (cooked_state::MAIN_AVAILABLE);

  /* This is run after finalization is done -- but not before.  If
     this task were submitted earlier, it would have to wait for
     finalization.  However, that would take a slot in the global
     thread pool, and if enough such tasks were submitted at once, it
     would cause a livelock.  */
  gdb::task_group finalizers ([this, warn] ()
  {
    m_state->set (cooked_state::FINALIZED);
    m_state->write_to_cache (index_for_writing (), warn);
    m_state->set (cooked_state::CACHE_DONE);
  });

  for (auto &shard : m_shards)
    {
      auto this_shard = shard.get ();
      finalizers.add_task ([=] () { this_shard->finalize (parent_maps); });
    }

  finalizers.start ();
}

cooked_index::~cooked_index ()
{
  /* Wait for index-creation to be done, though this one must also
     waited for by the per-BFD object to ensure the required data
     remains live.  */
  wait (cooked_state::CACHE_DONE);

  /* Remove our entry from the global list.  See the assert in the
     constructor to understand this.  */
  gdb_assert (is_main_thread ());
  active_vectors.erase (this);
}

/* See cooked-index.h.  */

dwarf2_per_cu *
cooked_index::lookup (unrelocated_addr addr)
{
  /* Ensure that the address maps are ready.  */
  wait (cooked_state::MAIN_AVAILABLE, true);
  for (const auto &shard : m_shards)
    {
      dwarf2_per_cu *result = shard->lookup (addr);
      if (result != nullptr)
	return result;
    }
  return nullptr;
}

/* See cooked-index.h.  */

std::vector<const addrmap *>
cooked_index::get_addrmaps ()
{
  /* Ensure that the address maps are ready.  */
  wait (cooked_state::MAIN_AVAILABLE, true);
  std::vector<const addrmap *> result;
  for (const auto &shard : m_shards)
    result.push_back (shard->m_addrmap);
  return result;
}

/* See cooked-index.h.  */

cooked_index::range
cooked_index::find (const std::string &name, bool completing)
{
  wait (cooked_state::FINALIZED, true);
  std::vector<cooked_index_shard::range> result_range;
  result_range.reserve (m_shards.size ());
  for (auto &shard : m_shards)
    result_range.push_back (shard->find (name, completing));
  return range (std::move (result_range));
}

/* See cooked-index.h.  */

const char *
cooked_index::get_main_name (struct obstack *obstack, enum language *lang)
  const
{
  const cooked_index_entry *entry = get_main ();
  if (entry == nullptr)
    return nullptr;

  *lang = entry->lang;
  return entry->full_name (obstack, FOR_MAIN);
}

/* See cooked_index.h.  */

const cooked_index_entry *
cooked_index::get_main () const
{
  const cooked_index_entry *best_entry = nullptr;
  for (const auto &shard : m_shards)
    {
      const cooked_index_entry *entry = shard->get_main ();
      /* Choose the first "main" we see.  We only do this for names
	 not requiring canonicalization.  At this point in the process
	 names might not have been canonicalized.  However, currently,
	 languages that require this step also do not use
	 DW_AT_main_subprogram.  An assert is appropriate here because
	 this filtering is done in get_main.  */
      if (entry != nullptr)
	{
	  if ((entry->flags & IS_MAIN) != 0)
	    {
	      if (!language_requires_canonicalization (entry->lang))
		{
		  /* There won't be one better than this.  */
		  return entry;
		}
	    }
	  else
	    {
	      /* This is one that is named "main".  Here we don't care
		 if the language requires canonicalization, due to how
		 the entry is detected.  Entries like this have worse
		 priority than IS_MAIN entries.  */
	      if (best_entry == nullptr)
		best_entry = entry;
	    }
	}
    }

  return best_entry;
}

quick_symbol_functions_up
cooked_index::make_quick_functions () const
{
  return quick_symbol_functions_up (new cooked_index_functions);
}

/* See cooked-index.h.  */

void
cooked_index::dump (gdbarch *arch)
{
  auto_obstack temp_storage;

  gdb_printf ("  entries:\n");
  gdb_printf ("\n");

  size_t i = 0;
  for (const cooked_index_entry *entry : this->all_entries ())
    {
      QUIT;

      gdb_printf ("    [%zu] ((cooked_index_entry *) %p)\n", i++, entry);
      gdb_printf ("    name:       %s\n", entry->name);
      gdb_printf ("    canonical:  %s\n", entry->canonical);
      gdb_printf ("    qualified:  %s\n",
		  entry->full_name (&temp_storage, 0, "::"));
      gdb_printf ("    DWARF tag:  %s\n", dwarf_tag_name (entry->tag));
      gdb_printf ("    flags:      %s\n", to_string (entry->flags).c_str ());
      gdb_printf ("    DIE offset: %s\n", sect_offset_str (entry->die_offset));

      if ((entry->flags & IS_PARENT_DEFERRED) != 0)
	gdb_printf ("    parent:     deferred (%" PRIx64 ")\n",
		    entry->get_deferred_parent ());
      else if (entry->get_parent () != nullptr)
	gdb_printf ("    parent:     ((cooked_index_entry *) %p) [%s]\n",
		    entry->get_parent (), entry->get_parent ()->name);
      else
	gdb_printf ("    parent:     ((cooked_index_entry *) 0)\n");

      gdb_printf ("\n");
    }

  const cooked_index_entry *main_entry = this->get_main ();
  if (main_entry != nullptr)
    gdb_printf ("  main: ((cooked_index_entry *) %p) [%s]\n", main_entry,
		  main_entry->name);
  else
    gdb_printf ("  main: ((cooked_index_entry *) 0)\n");

  gdb_printf ("\n");
  gdb_printf ("  address maps:\n");
  gdb_printf ("\n");

  std::vector<const addrmap *> addrmaps = this->get_addrmaps ();
  for (i = 0; i < addrmaps.size (); ++i)
    {
      const addrmap *addrmap = addrmaps[i];

      gdb_printf ("    [%zu] ((addrmap *) %p)\n", i, addrmap);
      gdb_printf ("\n");

      if (addrmap == nullptr)
	continue;

      addrmap->foreach ([arch] (CORE_ADDR start_addr, const void *obj)
	{
	  QUIT;

	  const char *start_addr_str = paddress (arch, start_addr);

	  if (obj != nullptr)
	    {
	      const dwarf2_per_cu *per_cu
		= static_cast<const dwarf2_per_cu *> (obj);
	      gdb_printf ("      [%s] ((dwarf2_per_cu *) %p)\n",
			  start_addr_str, per_cu);
	    }
	  else
	    gdb_printf ("      [%s] ((dwarf2_per_cu *) 0)\n", start_addr_str);

	  return 0;
	});

      gdb_printf ("\n");
    }
}

/* Wait for all the index cache entries to be written before gdb
   exits.  */
static void
wait_for_index_cache (int)
{
  gdb_assert (is_main_thread ());
  for (cooked_index *item : active_vectors)
    item->wait_completely ();
}

/* A maint command to wait for the cache.  */

static void
maintenance_wait_for_index_cache (const char *args, int from_tty)
{
  wait_for_index_cache (0);
}

void _initialize_cooked_index ();
void
_initialize_cooked_index ()
{
#if GDB_SELF_TEST
  selftests::register_test ("cooked_index_entry::compare", test_compare);
#endif

  add_cmd ("wait-for-index-cache", class_maintenance,
	   maintenance_wait_for_index_cache, _("\
Wait until all pending writes to the index cache have completed.\n\
Usage: maintenance wait-for-index-cache"),
	   &maintenancelist);

  gdb::observers::gdb_exiting.attach (wait_for_index_cache, "cooked-index");
}
