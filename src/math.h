/*! \file math.h
    \brief Some math utilities
 
*/
#ifndef _MATH_H_
#include <math.h>

/*! Maximum of two scalars
 	@param v First scalar
 	@param x Second scalar
 	@returns The greates scalar */
#define MAX(v, x) ((v > x) ? v : x)

/*! Minimum of two scalars
 	@param v First scalar
 	@param x Second scalar
 	@returns The less scalar */
#define MIN(v, x) ((v < x) ? v : x)

/*! Triangle wave function with period 2.0
 	@param var Variable of result
 	@param value Position in function
 	@returns float named var containing the function value */
#define math_ramp(var, value)\
	float var = fmod(value, 2.0);\
	if (var > 1) var = 2.0-var;


unsigned long xorshf96(void);

#define _MATH_H_
#endif