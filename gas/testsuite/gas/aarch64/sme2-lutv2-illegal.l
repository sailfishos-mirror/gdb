[^ :]+: Assembler messages:
[^ :]+:[0-9]+: Error: start register out of range at operand 3 -- `luti4 { ?z20.b ?- ?z23.h ?},zt0,{ ?z13 ?- ?z14 ?}'
[^ :]+:[0-9]+: Error: type mismatch in vector register list at operand 1 -- `luti4 { ?z19.b,z23.b,z27.b,z31.h ?},zt0,{ ?z13 ?- ?z14 ?}'
[^ :]+:[0-9]+: Error: unexpected register type at operand 1 -- `movt z3,zt0'
[^ :]+:[0-9]+: Error: unexpected register type at operand 1 -- `movt z3\[0\],zt0'
[^ :]+:[0-9]+: Error: operand 1 must be an integer register -- `movt zt1,z25'
[^ :]+:[0-9]+: Error: expected a vector register at operand 1 -- `luti4 zt0,\{ ?z0.b ?- ?z3.b\ ?},\{ ?z0 ?- ?z1\ ?}'
[^ :]+:[0-9]+: Error: expected a register at operand 2 -- `luti4 \{ ?z0.b ?- ?z3.b\ ?},\{ ?z0.b ?- ?z1.b\ ?} ?\{ ?z0 ?- ?z1\ ?}'
[^ :]+:[0-9]+: Error: syntax error in register list at operand 1 -- `luti4 \{ ?\ ?},zt0,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: invalid vector register in list at operand 3 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?},zt0,\{ ?\ ?}'
[^ :]+:[0-9]+: Error: unexpected register type at operand 2 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?},z3,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: start register out of range at operand 1 -- `luti4 \{ ?z19.b ?- ?z22.b\ ?},zt0,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: start register out of range at operand 3 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?},zt0,\{ ?z13 ?- ?z14\ ?}'
[^ :]+:[0-9]+: Error: expected a vector register at operand 1 -- `luti4 zt0,{ ?z0.b,z4.b,z8.b,z12.b ?},{ ?z0 ?- ?z1 ?}'
[^ :]+:[0-9]+: Error: expected a register at operand 2 -- `luti4 { ?z0.b,z4.b,z8.b,z12.b ?},{ ?z0.b ?- ?z1.b ?} ?{ ?z0 ?- ?z1 ?}'
[^ :]+:[0-9]+: Error: invalid vector register in list at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0,{ ? ?}'
[^ :]+:[0-9]+: Error: unexpected register type at operand 2 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},z3,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: start register out of range at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0,{ ?z13 ?- ?z14 ?}'
[^ :]+:[0-9]+: Error: operand mismatch -- `luti4 \{ ?z20.s ?- ?z23.s\ ?},zt0,\{ ?z20 ?- ?z21\ ?}'
[^ :]+:[0-9]+: Info:    did you mean this\?
[^ :]+:[0-9]+: Info:    	luti4 {z20.b-z23.b}, zt0, {z20-z21}
[^ :]+:[0-9]+: Error: operand mismatch -- `luti4 { ?z19.s,z23.s,z27.s,z31.s ?},zt0,{ ?z20 ?- ?z21 ?}'
[^ :]+:[0-9]+: Info:    did you mean this\?
[^ :]+:[0-9]+: Info:    	luti4 {z19.b, z23.b, z27.b, z31.b}, zt0, {z20-z21}
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `movt zt0.b,z31'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `movt zt0.b\[0,mul vl\],z31'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `movt zt0'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `movt zt0\[1,mul vl\]'
[^ :]+:[0-9]+: Error: unexpected characters following instruction at operand 2 -- `movt zt0,z23,z31'
[^ :]+:[0-9]+: Error: unexpected characters following instruction at operand 2 -- `movt zt0\[1,mul vl\],z23,z31'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?}'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 3 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?},zt0'
[^ :]+:[0-9]+: Error: unexpected characters following instruction at operand 3 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?},zt0,\{ ?z12 ?- ?z13\ ?},\{ ?z20 ?- ?z21\ ?}'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?}'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0 ?{ ?z12 ?- ?z13 ?},{ ?z20 ?- ?z21 ?}'
[^ :]+:[0-9]+: Error: operand 1 must be an integer register -- `movt zy0,z16'
[^ :]+:[0-9]+: Error: operand 1 must be an integer register -- `movt zt1,z16'
[^ :]+:[0-9]+: Error: operand 1 must be an integer register -- `movt zy0\[1,mul vl\],z16'
[^ :]+:[0-9]+: Error: expected an SVE vector register at operand 2 -- `movt zt0,y16'
[^ :]+:[0-9]+: Error: expected an SVE vector register at operand 2 -- `movt zt0\[1,mul vl\],y16'
[^ :]+:[0-9]+: Error: operand 1 must be a list of SVE vector registers -- `luti4 \{ ?z20.b ?- ?y23.b\ ?},zt0,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: unexpected character `x' in element size at operand 1 -- `luti4 \{ ?z20.x ?- ?z23.b\ ?},zt0,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: expected a register at operand 2 -- `luti4 \{ ?z20.b ?- ?z23.b\ ?},zy0,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: operand 1 must be a list of SVE vector registers -- `luti4 \{ ?z20.b ?- ?y23.b\ ?},zt0,\{ ?y12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: operand 1 must be a list of SVE vector registers -- `luti4 { ?z19.b,z23.b,z27.b,y31.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: unexpected character `x' in element size at operand 1 -- `luti4 { ?z19.x,z23.b,z27.b,z31.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: expected a register at operand 2 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zy0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: invalid vector register in list at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0,{ ?y12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: missing type suffix at operand 1 -- `luti4 \{ ?z20 ?- ?z23.b\ ?},zt0,\{ ?z12 ?- ?z13\ ?}'
[^ :]+:[0-9]+: Error: type mismatch in vector register list at operand 1 -- `luti4 { ?z19.b,z23,z27.b,z31.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: operand 1 must be an integer register -- `movt zt,z25.b'
[^ :]+:[0-9]+: Error: end of vector register list not found at operand 3 -- `luti4 { ?z20.b ?- ?z23.b ?},zt0,{ ?z12.b ?- ?z13.b ?}'
[^ :]+:[0-9]+: Error: end of vector register list not found at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0,{ ?z12.b ?- ?z13.b ?}'
[^ :]+:[0-9]+: Error: operand 2 must be an SVE vector register -- `movt zt0,z25\[1\]'
[^ :]+:[0-9]+: Error: operand 2 must be an SVE vector register -- `movt zt0,z25\[1,mul vl\]'
[^ :]+:[0-9]+: Error: expected an SVE vector register at operand 2 -- `movt zt0\[2,mul vl\],z32'
[^ :]+:[0-9]+: Error: register element index out of range 0 to 3 at operand 1 -- `movt zt0\[4,mul vl\],z25'
[^ :]+:[0-9]+: Error: register element index out of range 0 to 3 at operand 1 -- `movt zt0\[-1,mul vl\],z25'
[^ :]+:[0-9]+: Error: expected an SVE vector register at operand 1 -- `luti4 { ?z32.b ?- ?z36.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: invalid vector register in list at operand 3 -- `luti4 { ?z20.b ?- ?z23.b ?},zt0,{ ?z32 ?- ?z33 ?}'
[^ :]+:[0-9]+: Error: expected an SVE vector register at operand 1 -- `luti4 { ?z32.b,z36.b,z40.b,z44.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: invalid vector register in list at operand 3 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0,{ ?z32 ?- ?z33 ?}'
[^ :]+:[0-9]+: Error: too many registers in vector register list at operand 1 -- `luti4 { ?z20.b ?- ?z24.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: expected a list of 4 registers at operand 1 -- `luti4 { ?z20.b ?- ?z22.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: invalid register stride at operand 1 -- `luti4 { ?z20.b ?- ?z23.b ?},zt0,{ ?z12 ?- ?z14 ?}'
[^ :]+:[0-9]+: Error: invalid range in vector register list at operand 3 -- `luti4 { ?z20.b ?- ?z23.b ?},zt0,{ ?z12 ?- ?z12 ?}'
[^ :]+:[0-9]+: Error: invalid register list at operand 1 -- `luti4 { ?z19.b,z24.b,z27.b,z31.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: invalid register list at operand 1 -- `luti4 { ?z19.b,z22.b,z27.b,z31.b ?},zt0,{ ?z12 ?- ?z13 ?}'
[^ :]+:[0-9]+: Error: the register list must have a stride of 1 at operand 1 -- `luti4 { ?z19.b,z23.b,z27.b,z31.b ?},zt0,{ ?z12 ?- ?z14 ?}'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `movt zt0\[0\], ?z2'
[^ :]+:[0-9]+: Error: comma expected between operands at operand 2 -- `movt zt0\[1\], ?z2'
