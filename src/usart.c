#include "uart.h"
#include <math.h>

void usart_set_defaults(struct usart* usart) {
	DEFAULT(usart->configuration->baudrate, 115200);
	DEFAULT(usart->configuration->databits, 8);
	DEFAULT(usart->configuration->parity, USART_PARITY_NONE);
	DEFAULT(usart->configuration->stopbits, USART_STOPBITS_1);
	DEFAULT(usart->configuration->mode, USART_MODE_RX);
	DEFAULT(usart->configuration->flowcontrol, USART_FLOWCONTROL_NONE);
}




void usart_configure_gpio(struct usart* usart) {
	if (usart->configuration->tx_pin) {
		DEFAULT(usart->configuration->tx_pin->configuration->mode, GPIO_MODE_OUTPUT_2_MHZ);
		DEFAULT(usart->configuration->tx_pin->configuration->configuration, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL);
	}
	if (usart->configuration->rx_pin) {
		DEFAULT(usart->configuration->rx_pin->configuration->mode, GPIO_MODE_INPUT);
		DEFAULT(usart->configuration->rx_pin->configuration->configuration, GPIO_CNF_INPUT_FLOAT);
	}
}


//TODO: Check if we are inited already

void usart_init(struct usart* usart, enum hw_init_state state) {

	if (state == HW_INIT_RCC) {
		usart_configure_gpio(usart);
		rcc_periph_clock_enable(usart->configuration->rcc);
	}

	if (state == HW_INIT_PRE_NVIC) {
		usart_set_defaults(usart);
		usart_set_baudrate(usart->configuration->usart, usart->configuration->baudrate);
		usart_set_databits(usart->configuration->usart, usart->configuration->databits);
		usart_set_stopbits(usart->configuration->usart, usart->configuration->stopbits);
		usart_set_mode(usart->configuration->usart, usart->configuration->mode);
		usart_set_parity(usart->configuration->usart, usart->configuration->parity);
		usart_set_flow_control(usart->configuration->usart, usart->configuration->flowcontrol);

	}

	if (state == HW_INIT_POST_INIT) {
		usart_enable(usart->configuration->usart);
	}

	if(usart->configuration->tx_pin) {
		gpio_pin_init(usart->configuration->tx_pin, state);
	}

	if(usart->configuration->rx_pin) {
		gpio_pin_init(usart->configuration->rx_pin, state);
	}

}

void usart_blocking_send(struct usart* usart, volatile void* data, int length) {

	uint8_t* ptr = (uint8_t*)data;
	for (int i=0; i<length; i++) {
		usart_send_blocking(usart->configuration->usart, ptr[i]);

	}

}

void usart_blocking_strz(struct usart* usart, volatile char* ptr) {
	while(*ptr) {
		usart_send_blocking(usart->configuration->usart, *ptr++);
	}

}

void usart_blocking_int(struct usart* usart, int data) {
	int div = 1000000000;
	int num;
	int in_range=0;
	if (data < 0) {
		usart_send_blocking(usart->configuration->usart, '-');
		data = -data;
	}
	for (int i=10; i; i--) {
		num = (data / div) % 10;
		if (num && !in_range) {
			in_range=1;
		}
		if (in_range || i==1) {
			usart_send_blocking(usart->configuration->usart, '0' + num);
		}
		div/=10;
	}
}

void usart_blocking_int_zp(struct usart* usart, int data, int dp) {
	int div = pow(10, dp-1);
	int num;
	if (data < 0) {
		usart_send_blocking(usart->configuration->usart, '-');
		data = -data;
	}
	for (int i=dp; i; i--) {
		num = (data / div) % 10;
		usart_send_blocking(usart->configuration->usart, '0' + num);
		div/=10;
	}
}


void usart_blocking_float(struct usart* usart, float data) {

	usart_blocking_int(usart, (int)data);
	usart_send_blocking(usart->configuration->usart, '.');
	usart_blocking_int_zp(usart, (int)(fabs(data)*1000) % 1000, 3);

	// if (data) {
	// 	float l10 = floor(log10f(fabs(data)));
	// 	float n = data / powf(10.0, l10);
	// 	usart_send_blocking(usart->configuration->usart, '0' + ((int)floor(n)));
	// 	usart_send_blocking(usart->configuration->usart, '.');
	// 	usart_send_blocking(usart->configuration->usart, '0' + ((int)floor(n*10.0)) % 10);
	// 	usart_send_blocking(usart->configuration->usart, '0' + ((int)floor(n*100.0)) % 10);
	// 	usart_send_blocking(usart->configuration->usart, '0' + ((int)floor(n*1000.0)) % 10);
	// 	usart_send_blocking(usart->configuration->usart, 'E');
	// 	usart_send_blocking(usart->configuration->usart, l10 > 0 ? '+' : '-');
	// 	usart_blocking_int(usart, (int)l10);


	// } else {
	// 	usart_blocking_str(usart, "0.000");		
	// }
}
