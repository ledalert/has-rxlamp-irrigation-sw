// #include "msg.h"
// #include "time.h"
#include "hw.h"
#include "ws2812.h"


struct gpio_port led_port = {
	.configuration = &((struct gpio_port_config) {
		.port = GPIOB,
		.rcc = RCC_GPIOB,
	}),
};

struct gpio_pin led_pin = {
	.configuration = &((struct gpio_pin_config) {
		.port = &led_port,
		.pin = GPIO15,
	}),
};


struct timer led_ccr_timer = {
	.configuration = &((struct timer_config) {
		.timer = TIM1,
		.rcc = RCC_TIM1,
	}),
};


struct timer_ccr led_ccr = {
	.configuration = &((struct timer_ccr_config) {
		.timer = &led_ccr_timer,
		.channel = TIM_OC3N,
	}),
};


struct ws2812 led = {
	.configuration = &((struct ws2812_config) {
		.ccr = &led_ccr,
		.pin = &led_pin,
		.frequency = 800000,
		.bit0 = 6,	//TODO: verifiera
		.bit1 = 14,
	}),
};




void hw_init_state(enum hw_init_state state) {
	ws2812_init(&led, state);
}


int main(void) {

	rcc_clock_setup_in_hsi_out_24mhz();
	rcc_periph_clock_enable(RCC_AFIO);

	hw_init();

	return 0;
}