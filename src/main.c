// #include "msg.h"
// #include "time.h"
#include "hw.h"
#include "ws2812_macro.h"

#include <libopencm3/stm32/usart.h>


#define GPIO_PB15 GPIOB, RCC_GPIOB, GPIO15
#define TIMER1_CCR3N TIM1, RCC_TIM1, RCC_DMA1, TIM_OC3N, &TIM1_CCR3, DMA_CHANNEL6, TIM_DIER_CC3DE

WS2812_INSTANCE(MyLED, TIMER1_CCR3N, GPIO_PB15, 1);


void hw_init_state(enum hw_init_state state) {
	ws2812_init(&MyLED, state);
}


void uart_send_data(volatile void* data, int length) {

	uint8_t* ptr = (uint8_t*)data;
	for (int i=0; i<length; i++) {
		usart_send_blocking(USART1, ptr[i]);

	}

}

void DEBUG_PRINT_HEX(int value) {	
	const uint8_t hexdig[16] = "0123456789ABCDEF";
	uint8_t* ptr = (uint8_t*)&value;
	uart_send_data("0x", 2);

	for (int i=3; i>=0; i--) {
		usart_send_blocking(USART1, hexdig[ptr[i] >> 4]);
		usart_send_blocking(USART1, hexdig[ptr[i] & 0xF]);
	}
}

void init_usart() {
	usart_set_baudrate(USART1, 921600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_RX | USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);


}

#define DEBUG_PRINT(msg) uart_send_data(msg, sizeof(msg)-1);


int main(void) {

	rcc_clock_setup_in_hsi_out_24mhz();
	rcc_periph_clock_enable(RCC_AFIO);

	// Debug stuff
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);

	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	init_usart();
	DEBUG_PRINT("Initializing hardware\n");

	DEBUG_PRINT("ws2812 dma register: ");
	DEBUG_PRINT_HEX((uint32_t) MyLED.configuration->ccr->configuration->reg);
	DEBUG_PRINT("\n");

	// End debug stuff


	hw_init();

	MyLED.led_buffer[0] = (struct ws2812_rgb){50, 255, 0};
	while(1) {
		DEBUG_PRINT("In loop\n");
		ws2812_update(&MyLED);
		for (volatile int d=0; d<1000000;d++);
	}

	return 0;
}