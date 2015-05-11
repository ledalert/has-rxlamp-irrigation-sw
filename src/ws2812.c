#include "ws2812.h"

#define DEFAULT(value, default) value = value ? value : default

//debug
void uart_send_data(volatile void* data, int length);
void DEBUG_PRINT_HEX(int value);
#define DEBUG_PRINT(msg) uart_send_data(msg, sizeof(msg)-1);



void ws2812_configure_timer(struct ws2812* led) {
	DEBUG_PRINT("Configuring timer for ws2812\n");
	DEFAULT(led->configuration->ccr->configuration->timer->configuration->auto_reload, rcc_apb1_frequency / led->configuration->frequency);
}

void ws2812_configure_gpio(struct ws2812* led) {
	DEBUG_PRINT("Configuring GPIO for ws2812\n");
	DEFAULT(led->configuration->pin->configuration->mode, GPIO_MODE_OUTPUT_2_MHZ);
	DEFAULT(led->configuration->pin->configuration->configuration, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN);
}

void ws2812_set_defaults(struct ws2812* led) {
	DEBUG_PRINT("Configuring ws2812\n");
	DEFAULT(led->configuration->frequency, 800000);
	DEFAULT(led->configuration->bit0, 6);
	DEFAULT(led->configuration->bit1, 14);
}


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


void ws2812_process_buffer(struct ws2812* led) {

	DEBUG_PRINT("processing ws2812 buffer\n");

	volatile uint8_t* ptr=led->pwm_buffer;
	for (int l=0;l<led->led_count;l++) {

		for (int bit=7;bit>=0;bit--) { *ptr++ = led->led_buffer[l].g & (1 << bit) ? led->configuration->bit1 : led->configuration->bit0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led->led_buffer[l].r & (1 << bit) ? led->configuration->bit1 : led->configuration->bit0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led->led_buffer[l].b & (1 << bit) ? led->configuration->bit1 : led->configuration->bit0; }

	}

}


void ws2812_update(struct ws2812* led) {

	ws2812_process_buffer(led);

	DEBUG_PRINT("sending data to ws2812 using register ");
	DEBUG_PRINT_HEX((uint32_t)led->configuration->ccr->configuration->reg);

	DEBUG_PRINT(" buffer ");
	DEBUG_PRINT_HEX((uint32_t)led->pwm_buffer);

	DEBUG_PRINT(" number of LED ");
	DEBUG_PRINT_HEX((uint32_t)led->led_count);
	DEBUG_PRINT("\n")

	dma1_transmit_8_32((uint32_t) led->pwm_buffer, (uint32_t) led->configuration->ccr->configuration->reg, 24*led->led_count+1, led->configuration->ccr->configuration->dma_channel);
	DEBUG_PRINT("Activating dma\n");

	TIM_DIER(led->configuration->ccr->configuration->timer->configuration->timer) |= led->configuration->ccr->configuration->dma_enable_flag;

	DEBUG_PRINT("DMA Activated\n");
}
