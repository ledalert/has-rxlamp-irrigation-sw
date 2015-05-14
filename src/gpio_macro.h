/*! \file gpio_macro.h
    \brief Helper macros for GPIO
 
*/
#ifndef _GPIO_MACRO_H_

#define GPIO_PORT_INSTANCE(Name, Port, Rcc)\
struct gpio_port Name = {\
	.configuration = &((struct gpio_port_config){\
		.port = Port,\
		.rcc = Rcc,\
	}),\
};

#define GPIO_PIN_INSTANCE(Name, Port, Mode, Configuration, Pin)\
struct gpio_pin_config Name##_config = {\
	.port = Port,\
	.mode = Mode,\
	.configuration = Configuration,\
	.pin = Pin,\
};\
\
struct gpio_pin Name = {\
	.configuration = &Name##_config,\
};


#define _GPIO_MACRO_H_
#endif