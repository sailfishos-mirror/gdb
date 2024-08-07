/* LUTI2 (two registers) strided.  */
luti2	{ z0.b , z8.b }, zt0, z0[0]
LUTI2	{ Z0.B , Z8.B }, ZT0, Z0[0]
luti2	{ z7.b , z15.b }, zt0, z0[0]
luti2	{ z16.b , z24.b }, zt0, z0[0]
luti2	{ z23.b , z31.b }, zt0, z0[0]
luti2	{ z0.b , z8.b }, zt0, z31[0]
luti2	{ z0.b , z8.b }, zt0, z0[7]
luti2	{ z16.b , z24.b }, zt0, z31[0]
luti2	{ z16.b , z24.b }, zt0, z0[7]
luti2	{ z4.b , z12.b }, zt0, z20[4]
luti2	{ z20.b , z28.b }, zt0, z12[2]

luti2	{ z0.h , z8.h }, zt0, z0[0]
LUTI2	{ Z0.H , Z8.H }, ZT0, Z0[0]
luti2	{ z7.h , z15.h }, zt0, z0[0]
luti2	{ z16.h , z24.h }, zt0, z0[0]
luti2	{ z23.h , z31.h }, zt0, z0[0]
luti2	{ z0.h , z8.h }, zt0, z31[0]
luti2	{ z0.h , z8.h }, zt0, z0[7]
luti2	{ z16.h , z24.h }, zt0, z31[0]
luti2	{ z16.h , z24.h }, zt0, z0[7]
luti2	{ z4.h , z12.h }, zt0, z20[4]
luti2	{ z20.h , z28.h }, zt0, z12[2]

/* LUTI2 (four registers) strided.  */
luti2	{ z0.b , z4.b , z8.b , z12.b }, zt0, z0[0]
LUTI2	{ Z0.B , Z4.B, Z8.B , Z12.B }, ZT0, Z0[0]
luti2	{ z3.b , z7.b, z11.b, z15.b }, zt0, z0[0]
luti2	{ z16.b , z20.b , z24.b , z28.b }, zt0, z0[0]
luti2	{ z19.b , z23.b , z27.b , z31.b }, zt0, z0[0]
luti2	{ z0.b , z4.b , z8.b , z12.b }, zt0, z31[0]
luti2	{ z0.b , z4.b , z8.b , z12.b }, zt0, z0[3]
luti2	{ z16.b , z20.b , z24.b , z28.b }, zt0, z31[0]
luti2	{ z16.b , z20.b , z24.b , z28.b }, zt0, z0[3]
luti2	{ z2.b , z6.b, z10.b , z14.b }, zt0, z20[1]
luti2	{ z17.b , z21.b, z25.b , z29.b }, zt0, z10[2]

luti2	{ z0.h , z4.h , z8.h , z12.h }, zt0, z0[0]
LUTI2	{ Z0.H , Z4.H, Z8.H , Z12.H }, ZT0, Z0[0]
luti2	{ z3.h , z7.h, z11.h, z15.h }, zt0, z0[0]
luti2	{ z16.h , z20.h , z24.h , z28.h }, zt0, z0[0]
luti2	{ z19.h , z23.h , z27.h , z31.h }, zt0, z0[0]
luti2	{ z0.h , z4.h , z8.h , z12.h }, zt0, z31[0]
luti2	{ z0.h , z4.h , z8.h , z12.h }, zt0, z0[3]
luti2	{ z16.h , z20.h , z24.h , z28.h }, zt0, z31[0]
luti2	{ z16.h , z20.h , z24.h , z28.h }, zt0, z0[3]
luti2	{ z2.h , z6.h, z10.h , z14.h }, zt0, z20[1]
luti2	{ z17.h , z21.h, z25.h , z29.h }, zt0, z10[2]

/* LUTI4 (two registers) strided.  */
luti4	{ z0.b , z8.b }, zt0, z0[0]
LUTI4	{ Z0.B , Z8.B }, ZT0, Z0[0]
luti4	{ z7.b , z15.b }, zt0, z0[0]
luti4	{ z16.b , z24.b }, zt0, z0[0]
luti4	{ z23.b , z31.b }, zt0, z0[0]
luti4	{ z0.b , z8.b }, zt0, z31[0]
luti4	{ z0.b , z8.b }, zt0, z0[3]
luti4	{ z16.b , z24.b }, zt0, z31[0]
luti4	{ z16.b , z24.b }, zt0, z0[3]
luti4	{ z4.b , z12.b }, zt0, z20[1]
luti4	{ z20.b , z28.b }, zt0, z12[2]

luti4	{ z0.h , z8.h }, zt0, z0[0]
LUTI4	{ Z0.H , Z8.H }, ZT0, Z0[0]
luti4	{ z7.h , z15.h }, zt0, z0[0]
luti4	{ z16.h , z24.h }, zt0, z0[0]
luti4	{ z23.h , z31.h }, zt0, z0[0]
luti4	{ z0.h , z8.h }, zt0, z31[0]
luti4	{ z0.h , z8.h }, zt0, z0[3]
luti4	{ z16.h , z24.h }, zt0, z31[0]
luti4	{ z16.h , z24.h }, zt0, z0[3]
luti4	{ z4.h , z12.h }, zt0, z20[1]
luti4	{ z20.h , z28.h }, zt0, z12[2]

/* LUTI4 (four registers) strided.  */
luti4	{ z0.h , z4.h , z8.h , z12.h }, zt0, z0[0]
LUTI4	{ Z0.H , Z4.H, Z8.H , Z12.H }, ZT0, Z0[0]
luti4	{ z3.h , z7.h, z11.h, z15.h }, zt0, z0[0]
luti4	{ z16.h , z20.h , z24.h , z28.h }, zt0, z0[0]
luti4	{ z19.h , z23.h , z27.h , z31.h }, zt0, z0[0]
luti4	{ z0.h , z4.h , z8.h , z12.h }, zt0, z31[0]
luti4	{ z0.h , z4.h , z8.h , z12.h }, zt0, z0[1]
luti4	{ z16.h , z20.h , z24.h , z28.h }, zt0, z31[0]
luti4	{ z16.h , z20.h , z24.h , z28.h }, zt0, z0[1]
luti4	{ z2.h , z6.h, z10.h , z14.h }, zt0, z20[1]
luti4	{ z17.h , z21.h, z25.h , z29.h }, zt0, z10[0]
