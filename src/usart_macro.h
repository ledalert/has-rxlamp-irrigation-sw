/*! \file usart_macro.h
    \brief Helper macros for USART
 
*/
#ifndef _USART_MACRO_H_
#include "uart.h"


/*! Create usart, configured with 8 data bits, 1 stop bit, no parity */
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