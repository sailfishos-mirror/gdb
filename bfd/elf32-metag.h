/* Meta support for 32-bit ELF
   Copyright (C) 2013-2025 Free Software Foundation, Inc.
   Contributed by Imagination Technologies Ltd.

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

#ifndef _ELF32_METAG_H
#define _ELF32_METAG_H

extern int elf_metag_setup_section_lists
  (bfd *, struct bfd_link_info *);

extern void elf_metag_next_input_section
  (struct bfd_link_info *, asection *);

extern bool elf_metag_size_stubs
  (bfd *, bfd *, struct bfd_link_info *, bfd_signed_vma,
   asection * (*) (const char *, asection *), void (*) (void));

extern bool elf_metag_build_stubs
  (struct bfd_link_info *);

#endif  /* _ELF32_METAG_H */
