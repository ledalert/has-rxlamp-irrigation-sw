#include "led.h"
volatile static uint8_t led_output_buf[NUMBER_LEDS*24+1];	//42 x 24
volatile static struct color led_input_buf[NUMBER_LEDS];



void process_buffer() {
	volatile uint8_t* ptr=led_output_buf;
	for (int led=0;led<NUMBER_LEDS;led++) {

		for (int bit=7;bit>=0;bit--) { *ptr++ = led_input_buf[led].g & (1 << bit) ? BIT_1 : BIT_0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led_input_buf[led].r & (1 << bit) ? BIT_1 : BIT_0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led_input_buf[led].b & (1 << bit) ? BIT_1 : BIT_0; }

	}

}

void send_led_buffer() {
	//Send led data

	
	//dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);

//	TIM2_DIER &= ~TIM_DIER_CC1DE;	//Disable DMA transfer for CC1
//	usart_disable_rx_dma(USART1);


	dma1_transmit_8_32((uint32_t) led_output_buf, (uint32_t) &TIM1_CCR3, 24*NUMBER_LEDS+1, DMA_CHANNEL6 );
	TIM1_DIER |= TIM_DIER_CC3DE;	//Enable DMA transfer for CC1

}



void send_led_buffer_blocking() {
	send_led_buffer();
	while (!dma_get_interrupt_flag(DMA1, DMA_CHANNEL6, DMA_TCIF));

	TIM1_DIER &= ~TIM_DIER_CC3DE;	//Disable DMA transfer for CC1

}



void set_indicator_rgb(int r, int g, int b) {
	led_input_buf[0].r = r;
	led_input_buf[0].g = g;
	led_input_buf[0].b = b;

	process_buffer();
	send_led_buffer_blocking();

}


