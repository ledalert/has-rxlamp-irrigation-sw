#include "ws2812.h"




void ws2812_init(struct ws2812* led, enum hw_init_state state) {

	gpio_pin_init(led->configuration->pin, state);
	timer_ccr_init(led->configuration->ccr, state);

	if (state == HW_INIT_PRE_NVIC) {
		TIM_ARR(led->configuration->ccr->configuration->timer->configuration->timer) = (rcc_apb1_frequency / led->configuration->frequency) - 1;
	}


	if (state == HW_INIT_POST_INIT) {
		//LED was initialized
	}


}



