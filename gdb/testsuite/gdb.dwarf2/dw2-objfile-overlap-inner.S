/* This testcase is part of GDB, the GNU debugger.

   Copyright 2011-2025 Free Software Foundation, Inc.

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

	.text

	.globl _start
_start:

	.globl inner
	.type inner, %function
inner:
.Lbegin_inner:
	.int 0
.Lend_inner:
	.size inner, . - inner

/* Debug information */

	.section .debug_info
.Lcu1_begin:
	/* CU header */
	.4byte	.Lcu1_end - .Lcu1_start		/* Length of Compilation Unit */
.Lcu1_start:
	.2byte	2				/* DWARF Version */
	.4byte	.Labbrev1_begin			/* Offset into abbrev section */
	.byte	4				/* Pointer size */

	/* CU die */
	.uleb128 1				/* Abbrev: DW_TAG_compile_unit */
	.4byte	.Lline1_begin			/* DW_AT_stmt_list */
	.4byte  0				/* DW_AT_low_pc */
	.4byte	.Ldebug_ranges			/* DW_AT_ranges */
	.ascii	"inner.c\0"			/* DW_AT_name */
        .ascii  "/tmp\0"			/* DW_AT_comp_dir */
	.ascii	"GNU C 3.3.3\0"			/* DW_AT_producer */
	.byte	1				/* DW_AT_language (C) */

	/* inner */
	.uleb128	2			/* Abbrev: DW_TAG_subprogram */
	.byte		1			/* DW_AT_external */
	.byte		1			/* DW_AT_decl_file */
	.byte		1			/* DW_AT_decl_line */
	.ascii		"inner\0"		/* DW_AT_name */
	.4byte		.Lbegin_inner		/* DW_AT_low_pc */
	.4byte		.Lend_inner		/* DW_AT_high_pc */

	.byte		0			/* End of children of CU */

.Lcu1_end:

/* DW_AT_ranges.  */
	.section	.debug_ranges
.Ldebug_ranges:
	.4byte	.Lbegin_inner
	.4byte	.Lend_inner
	.4byte	0
	.4byte	0

/* Abbrev table */
	.section .debug_abbrev
.Labbrev1_begin:
	.uleb128	1			/* Abbrev code */
	.uleb128	0x11			/* DW_TAG_compile_unit */
	.byte		1			/* has_children */
	.uleb128	0x10			/* DW_AT_stmt_list */
	.uleb128	0x6			/* DW_FORM_data4 */
	.uleb128	0x11			/* DW_AT_low_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x55			/* DW_AT_ranges */
	.uleb128	0x6			/* DW_FORM_data4 */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128        0x1b                    /* DW_AT_comp_dir */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x25			/* DW_AT_producer */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x13			/* DW_AT_language */
	.uleb128	0xb			/* DW_FORM_data1 */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.uleb128	2			/* Abbrev code */
	.uleb128	0x2e			/* DW_TAG_subprogram */
	.byte		0			/* has_children */
	.uleb128	0x3f			/* DW_AT_external */
	.uleb128	0xc			/* DW_FORM_flag */
	.uleb128	0x3a			/* DW_AT_decl_file */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3b			/* DW_AT_decl_line */
	.uleb128	0xb			/* DW_FORM_data1 */
	.uleb128	0x3			/* DW_AT_name */
	.uleb128	0x8			/* DW_FORM_string */
	.uleb128	0x11			/* DW_AT_low_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.uleb128	0x12			/* DW_AT_high_pc */
	.uleb128	0x1			/* DW_FORM_addr */
	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

	.byte		0x0			/* Terminator */
	.byte		0x0			/* Terminator */

/* Line table */
	.section .debug_line
.Lline1_begin:
	.4byte		.Lline1_end - .Lline1_start	/* Initial length */
.Lline1_start:
	.2byte		2			/* Version */
	.4byte		.Lline1_lines - .Lline1_hdr	/* header_length */
.Lline1_hdr:
	.byte		1			/* Minimum insn length */
	.byte		1			/* default_is_stmt */
	.byte		1			/* line_base */
 	.byte		1			/* line_range */
	.byte		0x10			/* opcode_base */

	/* Standard lengths */
	.byte		0
	.byte		1
	.byte		1
	.byte		1
	.byte		1
	.byte		0
	.byte		0
	.byte		0
	.byte		1
	.byte		0
	.byte		0
	.byte		1
	.byte		0
	.byte		0
	.byte		0

	/* Include directories */
        .ascii  "/tmp\0"
	.byte		0

	/* File names */
	.ascii		"inner.c\0"
	.uleb128	1
	.uleb128	0
	.uleb128	0

	.byte		0

.Lline1_lines:
	.byte		0	/* DW_LNE_set_address */
	.uleb128	5
	.byte		2
	.4byte		.Lbegin_inner

	.byte		3	/* DW_LNS_advance_line */
	.sleb128	1	/* ... to 2 */

	.byte		1	/* DW_LNS_copy */

	.byte		0	/* DW_LNE_set_address */
	.uleb128	5
	.byte		2
	.4byte		.Lend_inner

	.byte		0	/* DW_LNE_end_of_sequence */
	.uleb128	1
	.byte		1

.Lline1_end:
#ifdef __arm__
	.section	.note.GNU-stack,"",%progbits
#else
	.section	.note.GNU-stack,"",@progbits
#endif
