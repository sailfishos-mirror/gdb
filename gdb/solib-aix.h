/* Copyright (C) 2013-2025 Free Software Foundation, Inc.

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

#ifndef GDB_SOLIB_AIX_H
#define GDB_SOLIB_AIX_H

#include "solib.h"

extern CORE_ADDR solib_aix_get_toc_value (CORE_ADDR pc);

/* Return a new solib_ops for AIX systems.  */

solib_ops_up make_aix_solib_ops ();

#endif /* GDB_SOLIB_AIX_H */
