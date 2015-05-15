/*! \file gpio_macro.h
    \brief Helper macros for GPIO
 
*/
#ifndef _GPIO_MACRO_H_

/*! Declares a new GPIO port 
	@param Name Variable name
	@portbasep{Port}
	@rccp{Rcc}
	@returns
		struct gpio_port Name
*/
#define GPIO_PORT_INSTANCE(Name, Port, Rcc)\
struct gpio_port Name = {\
	.configuration = &((struct gpio_port_config){\
		.port = Port,\
		.rcc = Rcc,\
	}),\
};

/*! Declares a new GPIO pin and GPIO pin config
	@param Name Variable name
	@portp{Port}
	@pinmodep{Mode}
	@pinconfp{Configuration}	
	@pinnop{Pin}
	@returns
		struct gpio_pin_config Name_config \n
		struct gpio_pin Name

*/
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

/*! Create GPIO pin with no configuration */
#define GPIO_PIN_NOCONF(Name, Port, Pin) GPIO_PIN_INSTANCE(Name, Port, 0, 0, Pin)



#define _GPIO_MACRO_H_
#endif