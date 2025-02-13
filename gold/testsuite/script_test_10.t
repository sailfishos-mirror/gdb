/* script_test_10.t -- test section order for gold.

   Copyright (C) 2010-2025 Free Software Foundation, Inc.
   Written by Viktor Kutuzov <vkutuzov@accesssoftek.com>.

   This file is part of gold.

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

SECTIONS
{
  .text : { *(.text) }
  .sec0 : { *(.sec0) }
  .sec1 : { *(.sec1) }
  .sec2 : { *(.sec2) }
  .secz : { *(.secz) }
  .sec3 : { *(.sec3) }
  .data : { *(.data) }
  .bss : { *(.bss) }
  /DISCARD/ : { *(.note.gnu.property) }
}

