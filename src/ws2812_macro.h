/*! \file ws2812_macro.h
    \brief Helper macros for ws2812
 
*/
#ifndef _WS2812_MACRO_H_
#include "ws2812.h"
#include "gpio_macro.h"
#include "timer_macro.h"

//WS2812_INSTANCE(StatusLED, &StatusLED_DMA, &StatusLED_pin);

/*! Create WS2812 and buffers with default settings 

	@namep{Name, WS2812}
	@param Pin Pin instance to use
	@param Ccr Timer CCR instance to use

	@returns
		@li struct ws2812_config Name_config
		@li struct ws2812_rgb Name_buf
		@li volatile uint8_t Name_pwmbuf
		@li struct ws2812 Name

*/
#define WS2812_SIMPLE_CONF(Name, Pin, Ccr, LEDCount)\
struct ws2812_config Name##_config = {\
	.ccr = Ccr,\
	.pin = Pin,	\
};\
\
struct ws2812_rgb Name##_buf[LEDCount];\
volatile uint8_t Name##_pwmbuf[LEDCount*24+1];\
\
struct ws2812 Name = {\
	.configuration = &Name##_config,\
	.led_buffer = Name##_buf,\
	.pwm_buffer = Name##_pwmbuf,\
	.led_count = LEDCount,\
};




#define _WS2812_MACRO_H_
#endif
