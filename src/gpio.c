/*! \file gpio.c
    \brief GPIO HAL implementation
 
*/
#include "gpio.h"



void gpio_port_init(struct gpio_port* port, enum hw_init_state state) {
	port->state = state;
	switch (state) {
		case HW_INIT_RCC:
			rcc_periph_clock_enable(port->configuration->rcc);
			break;
		default:
			break;

	} 

}

void gpio_pin_init(struct gpio_pin* pin, enum hw_init_state state) {
	pin->state = state;
	switch (state) {
		case HW_INIT_RCC:
			gpio_port_init(pin->configuration->port, state);
			break;

		case HW_INIT_GPIO:
			//TODO: check initial value
			gpio_set_mode(pin->configuration->port->configuration->port, pin->configuration->mode, pin->configuration->configuration, pin->configuration->pin);
			break;

		default:
			break;

	}
}

