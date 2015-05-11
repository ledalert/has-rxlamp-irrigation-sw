#ifndef _WS2812_H_

#include "gpio.h"
#include "timer.h"
#include "dma.h"

struct ws2812_config {
	struct timer_ccr* ccr;
	struct gpio_pin* pin;
	uint32_t frequency;
	uint32_t bit0;
	uint32_t bit1;
};

struct ws2812 {
	struct ws2812_config* configuration;
	struct ws2812_rgb* led_buffer;		//N
	uint8_t* pwm_buffer;				//24 * N + 1
	enum hw_init_state state;
	int led_count;
};

struct ws2812_rgb {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

void ws2812_init(struct ws2812* led, enum hw_init_state state);
void ws2812_update(struct ws2812* led);

#define _WS2812_H_
#endif