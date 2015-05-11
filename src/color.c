#include "color.h"

void colorHexagon(int hue, int *R, int *G, int *B) {
	int frac = hue >> 12;
	int ci = hue & 0xFFF;
	int cd = 4095 - ci;
	int cs = 4095;
	switch (frac) {
		case 0:	*R = cs;	*G = ci;	*B = 0; break;		//R1	G+	B0
		case 1:	*R = cd;	*G = cs;	*B = 0; break;		//R-	G1	B0
		case 2:	*R = 0;	*G = cs;	*B = ci; break;	//R0	G1	B+
		case 3:	*R = 0;	*G = cd;	*B = cs; break;	//R0	G-	B1
		case 4:	*R = ci;	*G = 0;	*B = cs; break;	//R+	G0	B1
		case 5:	*R = cs;	*G = 0;	*B = cd; break;	//R1	G0	B-
	}
}

