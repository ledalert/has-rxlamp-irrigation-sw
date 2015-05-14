/*! \file usart_macro.h
    \brief Helper macros for USART
 
*/
#ifndef _USART_MACRO_H_
#include "uart.h"


//USART_8N1(Serial, USART1, RCC_USART1, 921600, 0, &Serial_tx);
#define USART_8N1(Name, Usart, Rcc, Baudrate, Rx_pin, Tx_pin) \
	\
	struct usart_config Name##_config = {\
		.tx_pin = Tx_pin,\
		.rx_pin = Rx_pin,\
		.usart = Usart,\
		.rcc = Rcc,\
		.baudrate = Baudrate,\
	};\
	\
	struct usart Serial = {\
		.configuration = &Name##_config,\
	};



#define _USART_MACRO_H_
#endif