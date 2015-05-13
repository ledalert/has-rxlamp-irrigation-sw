#ifndef _MATH_H_
#include <math.h>
#define MAX(v, x) ((v > x) ? v : x)
#define MIN(v, x) ((v < x) ? v : x)

#define math_ramp(var, value)\
	float var = fmod(value, 2.0);\
	if (var > 1) var = 2.0-var;


unsigned long xorshf96(void);

#define _MATH_H_
#endif