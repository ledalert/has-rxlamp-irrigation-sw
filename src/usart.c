#include "uart.h"
void init_usart() {
	usart_set_baudrate(USART1, 921600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_RX | USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);


}

void uart_send_data(volatile void* data, int length) {

	uint8_t* ptr = (uint8_t*)data;
	for (int i=0; i<length; i++) {
		usart_send_blocking(USART1, ptr[i]);

	}

}

