#include "gpio.h"

//debug
void uart_send_data(volatile void* data, int length);
#define DEBUG_PRINT(msg) uart_send_data(msg, sizeof(msg)-1);


void gpio_port_init(struct gpio_port* port, enum hw_init_state state) {
	port->state = state;
	switch (state) {
		case HW_INIT_RCC:
			DEBUG_PRINT("Initializing gpio rcc\n");
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
			DEBUG_PRINT("Initializing gpio pin\n");
			gpio_set_mode(pin->configuration->port->configuration->port, pin->configuration->mode, pin->configuration->configuration, pin->configuration->pin);
			break;

		default:
			break;

	}
}

