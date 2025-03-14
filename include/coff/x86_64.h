/* COFF information for AMD 64.
   Copyright (C) 2006-2025 Free Software Foundation, Inc.

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
   MA 02110-1301, USA.
   
   Written by Kai Tietz, OneVision Software GmbH&CoKg.  */

#ifndef COFF_X86_64_H
#define COFF_X86_64_H

#include "coff/x86.h"

#define AMD64MAGIC	0x8664

/* Used in some .NET DLLs that target a specific OS.  */
#define AMD64_APPLE_MAGIC   (AMD64MAGIC ^ IMAGE_FILE_MACHINE_NATIVE_APPLE_OVERRIDE)
#define AMD64_FREEBSD_MAGIC (AMD64MAGIC ^ IMAGE_FILE_MACHINE_NATIVE_FREEBSD_OVERRIDE)
#define AMD64_LINUX_MAGIC   (AMD64MAGIC ^ IMAGE_FILE_MACHINE_NATIVE_LINUX_OVERRIDE)
#define AMD64_NETBSD_MAGIC  (AMD64MAGIC ^ IMAGE_FILE_MACHINE_NATIVE_NETBSD_OVERRIDE)

#define AMD64BADMAG(x) (   ((x).f_magic != AMD64MAGIC) \
                        && ((x).f_magic != AMD64_APPLE_MAGIC) \
                        && ((x).f_magic != AMD64_FREEBSD_MAGIC) \
                        && ((x).f_magic != AMD64_LINUX_MAGIC) \
                        && ((x).f_magic != AMD64_NETBSD_MAGIC))

#define IMAGE_NT_OPTIONAL_HDR64_MAGIC      0x20b

#define OMAGIC          0404    /* Object files, eg as output.  */
#define ZMAGIC          IMAGE_NT_OPTIONAL_HDR64_MAGIC    /* Demand load format, eg normal ld output 0x10b.  */
#define STMAGIC		0401	/* Target shlib.  */
#define SHMAGIC		0443	/* Host   shlib.  */

/* X86-64 relocations.  */
#define R_AMD64_ABS 		 0 /* Reference is absolute, no relocation is necessary.  */
#define R_AMD64_DIR64		 1 /* 64-bit address (VA).  */
#define R_AMD64_DIR32		 2 /* 32-bit address (VA) R_DIR32.  */
#define R_AMD64_IMAGEBASE	 3 /* 32-bit absolute ref w/o base R_IMAGEBASE.  */
#define R_AMD64_PCRLONG		 4 /* 32-bit relative address from byte following reloc R_PCRLONG.  */
#define R_AMD64_PCRLONG_1	 5 /* 32-bit relative address from byte distance 1 from reloc.  */
#define R_AMD64_PCRLONG_2	 6 /* 32-bit relative address from byte distance 2 from reloc.  */
#define R_AMD64_PCRLONG_3	 7 /* 32-bit relative address from byte distance 3 from reloc.  */
#define R_AMD64_PCRLONG_4	 8 /* 32-bit relative address from byte distance 4 from reloc.  */
#define R_AMD64_PCRLONG_5	 9 /* 32-bit relative address from byte distance 5 from reloc.  */
#define R_AMD64_SECTION		10 /* Section index.  */
#define R_AMD64_SECREL		11 /* 32 bit offset from base of section containing target R_SECREL.  */
#define R_AMD64_SECREL7		12 /* 7 bit unsigned offset from base of section containing target.  */
#define R_AMD64_TOKEN		13 /* 32 bit metadata token.  */
#define R_AMD64_PCRQUAD		14 /* Pseude PC64 relocation - Note: not specified by MS/AMD but need for gas pc-relative 64bit wide relocation generated by ELF.  */

/* i386 Relocations.  */

#define R_DIR32		 6
#define R_IMAGEBASE	 7
#define R_SECTION	10
#define R_SECREL32	11
#define R_RELBYTE	15
#define R_RELWORD	16
#define R_RELLONG	17
#define R_PCRBYTE	18
#define R_PCRWORD	19
#define R_PCRLONG	20

#endif /* COFF_X86_64_H */
