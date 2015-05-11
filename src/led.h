#ifndef _LED_H_
#include <stdint.h>

#define BIT_0 6
#define BIT_1 14
#define FREQ 800000

#define NUMBER_LEDS 1


#define LED_LIGHT_BLUE 50, 50, 100
#define LED_ORANGE 100, 50, 0
#define LED_GREEN 0, 100, 0


struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};




#define _LED_H_
#endif