/* IBM RS/6000 "XCOFF" back-end for BFD.
   Copyright (C) 2001-2025 Free Software Foundation, Inc.
   Written by Tom Rix
   Contributed by Red Hat Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"

bfd_cleanup xcoff64_core_p (bfd *);
bool xcoff64_core_file_matches_executable_p (bfd *, bfd *);
char *xcoff64_core_file_failing_command (bfd *);
int xcoff64_core_file_failing_signal (bfd *);

#ifdef AIX_5_CORE

#include "libbfd.h"

/* Aix 5.1 system include file.  */

/* Need to define this macro so struct ld_info64 get included.  */
#define __LDINFO_PTRACE64__
#include <sys/ldr.h>
#include <core.h>

/* The default architecture and machine for matching core files.  */
#define DEFAULT_ARCHITECTURE	bfd_arch_powerpc
#define DEFAULT_MACHINE		bfd_mach_ppc_620

#define	core_hdr(abfd)		((struct core_dumpxx *) abfd->tdata.any)

#define CHECK_FILE_OFFSET(s, v) \
  ((bfd_signed_vma)(v) < 0 || (bfd_signed_vma)(v) > (bfd_signed_vma)(s).st_size)

bfd_cleanup
xcoff64_core_p (bfd *abfd)
{
  enum bfd_architecture arch;
  unsigned long mach;
  struct core_dumpxx core, *new_core_hdr;
  struct stat statbuf;
  asection *sec;
  struct __ld_info64 ldinfo;
  bfd_vma ld_offset;
  bfd_size_type i;
  struct vm_infox vminfo;
  flagword flags;

  /* Get the header.  */
  if (bfd_seek (abfd, 0, SEEK_SET) != 0)
    goto xcoff64_core_p_error;

  if (sizeof core != bfd_read (&core, sizeof core, abfd))
    goto xcoff64_core_p_error;

  if (bfd_stat (abfd, &statbuf) < 0)
    goto xcoff64_core_p_error;

  /* Sanity checks
     c_flag has CORE_VERSION_1, Aix 4+
     c_entries = 0 for Aix 4.3+
     IS_PROC64 is a macro defined in procinfo.h, test for 64 bit process.

     We will still be confused if a Aix 4.3 64 bit core file is
     copied over to a Aix 5 machine.

     Check file header offsets

     See rs6000-core.c for comment on size of core
     If there isn't enough of a real core file, bail.  */

  if ((CORE_VERSION_1 != (core.c_flag & CORE_VERSION_1))
      || (0 != core.c_entries)
      || (! (IS_PROC64 (&core.c_u.U_proc)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_fdsinfox)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_loader)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_loader + core.c_lsize)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_thr)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_segregion)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_stack)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_stack + core.c_size)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_data)))
      || ((CHECK_FILE_OFFSET (statbuf, core.c_data + core.c_datasize)))
      || (! (core.c_flag & UBLOCK_VALID))
      || (! (core.c_flag & LE_VALID)))
    goto xcoff64_core_p_error;

  /* Check for truncated stack or general truncating.  */
  if ((! (core.c_flag & USTACK_VALID))
      || (core.c_flag & CORE_TRUNC))
    {
      bfd_set_error (bfd_error_file_truncated);

      return NULL;
    }

  new_core_hdr = bfd_alloc (abfd, sizeof (*new_core_hdr) + 1);
  if (NULL == new_core_hdr)
    return NULL;

  memcpy (new_core_hdr, &core, sizeof (*new_core_hdr));

  /* Ensure core_file_failing_command string is terminated.  This is
     just to stop buffer overflows on fuzzed files.  */
  ((char *) new_core_hdr)[sizeof (*new_core_hdr)] = 0;

  abfd->tdata.any = new_core_hdr;

  /* .stack section.  */
  flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS;
  sec = bfd_make_section_anyway_with_flags (abfd, ".stack", flags);
  if (NULL == sec)
    return NULL;

  sec->size = core.c_size;
  sec->vma = core.c_stackorg;
  sec->filepos = core.c_stack;

  /* .reg section for all registers.  */
  flags = SEC_HAS_CONTENTS | SEC_IN_MEMORY;
  sec = bfd_make_section_anyway_with_flags (abfd, ".reg", flags);
  if (NULL == sec)
    return NULL;

  sec->size = sizeof (struct __context64);
  sec->vma = 0;
  sec->filepos = 0;
  sec->contents = (bfd_byte *)&new_core_hdr->c_flt.r64;

  if (core.c_extctx)
    {
      /* vmx section.  */
      flags = SEC_HAS_CONTENTS;
      sec = bfd_make_section_anyway_with_flags (abfd, ".aix-vmx", flags);
      if (sec == NULL)
	return NULL;
      sec->size = 560;
      sec->vma = 0;
      sec->filepos = core.c_extctx;

      /* vmx section.  */
      flags = SEC_HAS_CONTENTS;
      sec = bfd_make_section_anyway_with_flags (abfd, ".aix-vsx", flags);
      if (sec == NULL)
	return NULL;
      sec->size = 256;
      sec->vma = 0;
      sec->filepos = core.c_extctx + 584;
    }

  /* .ldinfo section.
     To actually find out how long this section is in this particular
     core dump would require going down the whole list of struct
     ld_info's.   See if we can just fake it.  */
  flags = SEC_HAS_CONTENTS;
  sec = bfd_make_section_anyway_with_flags (abfd, ".ldinfo", flags);
  if (NULL == sec)
    return NULL;

  sec->size = core.c_lsize;
  sec->vma = 0;
  sec->filepos = core.c_loader;

  /* AIX 4 adds data sections from loaded objects to the core file,
     which can be found by examining ldinfo, and anonymously mmapped
     regions.  */

  /* .data section from executable.  */
  flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS;
  sec = bfd_make_section_anyway_with_flags (abfd, ".data", flags);
  if (NULL == sec)
    return NULL;

  sec->size = core.c_datasize;
  sec->vma = core.c_dataorg;
  sec->filepos = core.c_data;

  /* .data sections from loaded objects.  */
  ld_offset = core.c_loader;

  while (1)
    {
      if (bfd_seek (abfd, ld_offset, SEEK_SET) != 0)
	return NULL;

      if (sizeof (struct __ld_info64) !=
	  bfd_read (&ldinfo, sizeof (struct __ld_info64), abfd))
	return NULL;

      if (ldinfo.ldinfo_core)
	{
	  flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS;
	  sec = bfd_make_section_anyway_with_flags (abfd, ".data", flags);
	  if (NULL == sec)
	    return NULL;

	  sec->size = ldinfo.ldinfo_datasize;
	  sec->vma = ldinfo.ldinfo_dataorg;
	  sec->filepos = ldinfo.ldinfo_core;
	}

      if (0 == ldinfo.ldinfo_next)
	break;
      ld_offset += ldinfo.ldinfo_next;
    }

  /* .vmdata sections from anonymously mmapped regions.  */
  if (core.c_vmregions)
    {
      if (bfd_seek (abfd, core.c_vmm, SEEK_SET) != 0)
	return NULL;

      for (i = 0; i < core.c_vmregions; i++)
	if (sizeof (struct vm_infox) !=
	    bfd_read (&vminfo, sizeof (struct vm_infox), abfd))
	  return NULL;

      if (vminfo.vminfo_offset)
	{
	  flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS;
	  sec = bfd_make_section_anyway_with_flags (abfd, ".vmdata", flags);
	  if (NULL == sec)
	    return NULL;

	  sec->size = vminfo.vminfo_size;
	  sec->vma = vminfo.vminfo_addr;
	  sec->filepos = vminfo.vminfo_offset;
	}
    }

  /* Set the architecture and machine.  */
  arch = DEFAULT_ARCHITECTURE;
  mach = DEFAULT_MACHINE;
  bfd_default_set_arch_mach (abfd, arch, mach);

  return _bfd_no_cleanup;

 xcoff64_core_p_error:
  if (bfd_get_error () != bfd_error_system_call)
    bfd_set_error (bfd_error_wrong_format);

  return NULL;
}

/* Return `TRUE' if given core is from the given executable.  */

bool
xcoff64_core_file_matches_executable_p (bfd *core_bfd, bfd *exec_bfd)
{
  struct core_dumpxx core;
  char *path, *s;
  size_t alloc;
  const char *str1, *str2;
  bool return_value = false;

  /* Get the header.  */
  if (bfd_seek (core_bfd, 0, SEEK_SET) != 0)
    return return_value;

  if (sizeof (struct core_dumpxx) !=
      bfd_read (&core, sizeof (struct core_dumpxx), core_bfd))
    return return_value;

  if (bfd_seek (core_bfd, core.c_loader, SEEK_SET) != 0)
    return return_value;

  alloc = 100;
  path = bfd_malloc (alloc);
  if (path == NULL)
    return return_value;

  s = path;

  while (1)
    {
      if (bfd_read (s, 1, core_bfd) != 1)
	goto xcoff64_core_file_matches_executable_p_end_1;

      if (*s == '\0')
	break;
      ++s;
      if (s == path + alloc)
	{
	  char *n;

	  alloc *= 2;
	  n = bfd_realloc (path, alloc);
	  if (n == NULL)
	    goto xcoff64_core_file_matches_executable_p_end_1;

	  s = n + (path - s);
	  path = n;
	}
    }

  str1 = strrchr (path, '/');
  str2 = strrchr (bfd_get_filename (exec_bfd), '/');

  /* Step over character '/'.  */
  str1 = str1 != NULL ? str1 + 1 : path;
  str2 = str2 != NULL ? str2 + 1 : bfd_get_filename (exec_bfd);

  if (strcmp (str1, str2) == 0)
    return_value = true;

 xcoff64_core_file_matches_executable_p_end_1:
  free (path);
  return return_value;
}

char *
xcoff64_core_file_failing_command (bfd *abfd)
{
  struct core_dumpxx *c = core_hdr (abfd);
  char *return_value = 0;

  if (NULL != c)
    return_value = c->c_u.U_proc.pi_comm;

  return return_value;
}

int
xcoff64_core_file_failing_signal (bfd *abfd)
{
  struct core_dumpxx *c = core_hdr (abfd);
  int return_value = 0;

  if (NULL != c)
    return_value = c->c_signo;

  return return_value;
}

#else /* AIX_5_CORE */

bfd_cleanup
xcoff64_core_p (bfd *abfd ATTRIBUTE_UNUSED)
{
  bfd_set_error (bfd_error_wrong_format);
  return 0;
}

bool
xcoff64_core_file_matches_executable_p (bfd *core_bfd, bfd *exec_bfd)
{
  return generic_core_file_matches_executable_p (core_bfd, exec_bfd);
}

char *
xcoff64_core_file_failing_command (bfd *abfd ATTRIBUTE_UNUSED)
{
  return 0;
}

int
xcoff64_core_file_failing_signal (bfd *abfd ATTRIBUTE_UNUSED)
{
  return 0;
}

#endif /* AIX_5_CORE */
