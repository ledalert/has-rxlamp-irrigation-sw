#ifndef _GPIO_H_
#include "hw.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>


struct gpio_port {
	struct gpio_port_config* configuration;
	enum hw_init_state state;
};

struct gpio_port_config {
	uint32_t port;
	uint32_t rcc;
};

struct gpio_pin_config {
	struct gpio_port* port;
	uint32_t pin;
	uint32_t mode;
	uint32_t configuration;
	int initial;
};

struct gpio_pin {
	struct gpio_pin_config* configuration;
	enum hw_init_state state;
};

void gpio_pin_init(struct gpio_pin* pin, enum hw_init_state state);
void gpio_port_init(struct gpio_port* pin, enum hw_init_state state);

#define _GPIO_H_
#endif