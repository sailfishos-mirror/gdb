/* MI Command Set - breakpoint and watchpoint commands.
   Copyright (C) 2012-2025 Free Software Foundation, Inc.

   Contributed by Intel Corporation.

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

#ifndef GDB_MI_MI_CMD_BREAK_H
#define GDB_MI_MI_CMD_BREAK_H

#include "gdbsupport/scoped_restore.h"

/* Setup the reporting of the insertion of a new breakpoint or
   catchpoint.  */
scoped_restore_tmpl<int> setup_breakpoint_reporting (void);

#endif /* GDB_MI_MI_CMD_BREAK_H */
