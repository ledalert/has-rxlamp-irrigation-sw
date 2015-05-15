/*! \file ws2812.c
    \brief Implementation of WS2812 HAL
 
*/
#include "ws2812.h"






/*! Apply default configuration to the timer 
	@param led Bus to use
*/
void ws2812_configure_timer(struct ws2812* led) {
	DEFAULT(led->configuration->ccr->configuration->timer->auto_reload, rcc_apb1_frequency / led->configuration->frequency - 1);
}

/*! Apply default configuration to GPIO 
	@param led Bus to use
*/
void ws2812_configure_gpio(struct ws2812* led) {
	DEFAULT(led->configuration->pin->configuration->mode, GPIO_MODE_OUTPUT_2_MHZ);
	DEFAULT(led->configuration->pin->configuration->configuration, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN);
}

/*! Apply default configration to ws2812 
	@param led Bus to use
*/
void ws2812_set_defaults(struct ws2812* led) {
	DEFAULT(led->configuration->frequency, 800000);
	DEFAULT(led->configuration->bit0, 6);
	DEFAULT(led->configuration->bit1, 14);
}

/*! Initiate ws2812 bus to state 
	@param led Bus to init
	@param state New HW state
*/
void ws2812_init(struct ws2812* led, enum hw_init_state state) {


	if (state == HW_INIT_PRE_NVIC) {
		ws2812_configure_timer(led);
	}

	if (state == HW_INIT_GPIO) {
		ws2812_set_defaults(led);
		ws2812_configure_gpio(led);
	}


	gpio_pin_init(led->configuration->pin, state);
	timer_ccr_init(led->configuration->ccr, state);


	if (state == HW_INIT_POST_INIT) {
		//LED was initialized
	}


}

/*! Process LED buffer */
void ws2812_process_buffer(struct ws2812* led) {

	volatile uint8_t* ptr=led->pwm_buffer;
	for (int l=0;l<led->led_count;l++) {

		for (int bit=7;bit>=0;bit--) { *ptr++ = led->led_buffer[l].g & (1 << bit) ? led->configuration->bit1 : led->configuration->bit0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led->led_buffer[l].r & (1 << bit) ? led->configuration->bit1 : led->configuration->bit0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led->led_buffer[l].b & (1 << bit) ? led->configuration->bit1 : led->configuration->bit0; }

	}
 	*ptr = 0;	//Set last value to 0

}

/*! Send data to LEDs */
void ws2812_update(struct ws2812* led) {

	ws2812_process_buffer(led);
	dma_channel_send(led->configuration->ccr->configuration->dma, led->pwm_buffer, 24 * led->led_count + 1);
	// dma1_transmit_8_32((uint32_t) led->pwm_buffer, (uint32_t) led->configuration->ccr->configuration->reg, 24*led->led_count+1, led->configuration->ccr->configuration->dma_channel);
	// TIM_DIER(led->configuration->ccr->configuration->timer->configuration->timer) |= led->configuration->ccr->configuration->dma_enable_flag;

}

/*! Send data to LEDs, wait for completion */
void ws2812_update_blocking(struct ws2812* led) {
	ws2812_process_buffer(led);
	dma_channel_send_blocking(led->configuration->ccr->configuration->dma, led->pwm_buffer, 24 * led->led_count + 1);
}

/*! Set LED buffer index to color r, g, b (0-255) */
void ws2812_set_led(struct ws2812* led, int index, int r, int g, int b) {
	led->led_buffer[index] = (struct ws2812_rgb) {r, g, b};
}