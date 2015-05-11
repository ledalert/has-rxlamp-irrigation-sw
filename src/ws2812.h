#ifndef _WS2812_H_

#include "gpio.h"
#include "timer.h"

struct ws2812_config {
	struct timer_ccr* ccr;
	struct gpio_pin* pin;
	uint32_t frequency;
	uint32_t bit0;
	uint32_t bit1;
};

struct ws2812 {
	struct ws2812_config* configuration;
	enum hw_init_state state;
};

void ws2812_init(struct ws2812* led, enum hw_init_state state);

#define _WS2812_H_
#endif