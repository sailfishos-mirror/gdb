#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS divide and multiply macros with --trap
#as: -32 --trap
#warning_output: brtr-opt.l
#source: brtr-opt.s

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 000001f4 	teq	zero,zero,0x7
[0-9a-f]+ <[^>]*> 000001f4 	teq	zero,zero,0x7
[0-9a-f]+ <[^>]*> 004001f4 	teq	v0,zero,0x7
[0-9a-f]+ <[^>]*> 0082001a 	div	zero,a0,v0
[0-9a-f]+ <[^>]*> 2401ffff 	li	at,-1
[0-9a-f]+ <[^>]*> 14410002 	bne	v0,at,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 3c018000 	lui	at,0x8000
[0-9a-f]+ <[^>]*> 008101b4 	teq	a0,at,0x6
[0-9a-f]+ <[^>]*> 00003010 	mfhi	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 004001f4 	teq	v0,zero,0x7
[0-9a-f]+ <[^>]*> 0082001b 	divu	zero,a0,v0
[0-9a-f]+ <[^>]*> 00003010 	mfhi	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 02318826 	xor	s1,s1,s1
[0-9a-f]+ <[^>]*> 00820018 	mult	a0,v0
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 000637c3 	sra	a2,a2,0x1f
[0-9a-f]+ <[^>]*> 00000810 	mfhi	at
[0-9a-f]+ <[^>]*> 00c101b6 	tne	a2,at,0x6
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 24010002 	li	at,2
[0-9a-f]+ <[^>]*> 00810019 	multu	a0,at
[0-9a-f]+ <[^>]*> 00000810 	mfhi	at
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 002001b6 	tne	at,zero,0x6
[0-9a-f]+ <[^>]*> 0007000d 	break	0x7
[0-9a-f]+ <[^>]*> 0007000d 	break	0x7
[0-9a-f]+ <[^>]*> 14400002 	bnez	v0,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 0082001a 	div	zero,a0,v0
[0-9a-f]+ <[^>]*> 0007000d 	break	0x7
[0-9a-f]+ <[^>]*> 2401ffff 	li	at,-1
[0-9a-f]+ <[^>]*> 14410004 	bne	v0,at,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 3c018000 	lui	at,0x8000
[0-9a-f]+ <[^>]*> 14810002 	bne	a0,at,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 0006000d 	break	0x6
[0-9a-f]+ <[^>]*> 00003010 	mfhi	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 14400002 	bnez	v0,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 0082001b 	divu	zero,a0,v0
[0-9a-f]+ <[^>]*> 0007000d 	break	0x7
[0-9a-f]+ <[^>]*> 00003010 	mfhi	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 02318826 	xor	s1,s1,s1
[0-9a-f]+ <[^>]*> 00820018 	mult	a0,v0
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 000637c3 	sra	a2,a2,0x1f
[0-9a-f]+ <[^>]*> 00000810 	mfhi	at
[0-9a-f]+ <[^>]*> 10c10002 	beq	a2,at,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 0006000d 	break	0x6
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 24010002 	li	at,2
[0-9a-f]+ <[^>]*> 00810019 	multu	a0,at
[0-9a-f]+ <[^>]*> 00000810 	mfhi	at
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 10200002 	beqz	at,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 0006000d 	break	0x6
[0-9a-f]+ <[^>]*> 000001f4 	teq	zero,zero,0x7
[0-9a-f]+ <[^>]*> 000001f4 	teq	zero,zero,0x7
[0-9a-f]+ <[^>]*> 004001f4 	teq	v0,zero,0x7
[0-9a-f]+ <[^>]*> 0082001a 	div	zero,a0,v0
[0-9a-f]+ <[^>]*> 2401ffff 	li	at,-1
[0-9a-f]+ <[^>]*> 14410002 	bne	v0,at,[0-9a-f]+ <[^>]*>
[0-9a-f]+ <[^>]*> 3c018000 	lui	at,0x8000
[0-9a-f]+ <[^>]*> 008101b4 	teq	a0,at,0x6
[0-9a-f]+ <[^>]*> 00003010 	mfhi	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 004001f4 	teq	v0,zero,0x7
[0-9a-f]+ <[^>]*> 0082001b 	divu	zero,a0,v0
[0-9a-f]+ <[^>]*> 00003010 	mfhi	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 02318826 	xor	s1,s1,s1
[0-9a-f]+ <[^>]*> 00820018 	mult	a0,v0
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 000637c3 	sra	a2,a2,0x1f
[0-9a-f]+ <[^>]*> 00000810 	mfhi	at
[0-9a-f]+ <[^>]*> 00c101b6 	tne	a2,at,0x6
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 02108026 	xor	s0,s0,s0
[0-9a-f]+ <[^>]*> 24010002 	li	at,2
[0-9a-f]+ <[^>]*> 00810019 	multu	a0,at
[0-9a-f]+ <[^>]*> 00000810 	mfhi	at
[0-9a-f]+ <[^>]*> 00003012 	mflo	a2
[0-9a-f]+ <[^>]*> 002001b6 	tne	at,zero,0x6
	\.\.\.
