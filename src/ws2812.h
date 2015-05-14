/*! \file ws2812.h
    \brief WS2812 HAL include file
 
*/
#ifndef _WS2812_H_

#include "gpio.h"
#include "timer.h"
#include "dma.h"

/*! Configuration for WS2812 */
struct ws2812_config {
	struct timer_ccr* ccr;	/*!< What CCR to use */
	struct gpio_pin* pin;	/*!< What GPIO pin to use */
	uint32_t frequency;		/*!< Frequency of data, defaults to 800kHz */
	uint32_t bit0;			/*!< Length of bit 0 for PWM */
	uint32_t bit1;			/*!< Length of bit 1 for PWM */
};

/*! WS2812 Instance */
struct ws2812 {
	struct ws2812_config* configuration;		/*!< Pointer to configuration */
	struct ws2812_rgb* led_buffer;				/*!< Pointer to LED array @note This should contain as many elements as there are LEDs */
	volatile uint8_t* pwm_buffer;				/*!< Pointer to buffer for PWM data @note This should contain 24 * N + 1 elements where N is number of LEDs */
	enum hw_init_state state;					/*!< Device state */
	int led_count;								/*!< Number of LEDs connected to the bus */
};

/*! 24 bit color */
struct ws2812_rgb {
	uint8_t r;	/*!< Red 0-255 */
	uint8_t g;	/*!< Green 0-255 */
	uint8_t b;	/*!< Blue 0-255 */
};

void ws2812_init(struct ws2812* led, enum hw_init_state state);
void ws2812_update(struct ws2812* led);
void ws2812_set_led(struct ws2812* led, int index, int r, int g, int b);
void ws2812_update_blocking(struct ws2812* led);

#define _WS2812_H_
#endif