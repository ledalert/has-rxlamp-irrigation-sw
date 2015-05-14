/*! \file usart_macro.h
    \brief Helper macros for USART
 
*/
#ifndef _USART_MACRO_H_
#include "uart.h"


#define USART_TXONLY_INSTANCE(Name, Usart, Usart_Rcc, Baudrate, Databits, Parity, Stopbits, Flowcontrol, TXPin_Port, TXPin_Pin)\
\
struct gpio_pin_config Name##_txpin_config = {\
	.port = TXPin_Port,\
	.pin = TXPin_Pin,\
};\
\
struct gpio_pin Name##_txpin = {\
	.configuration = &Name##_txpin_config,\
};\
\
struct usart_config Name##_config = {\
	.usart = Usart,\
	.rcc = Usart_Rcc,\
	.tx_pin = &Name##_txpin,\
	.baudrate = Baudrate,\
	.databits = Databits,\
	.parity = Parity,\
	.stopbits = Stopbits,\
	.flowcontrol = Flowcontrol,\
	.mode = USART_MODE_TX,\
};\
\
struct usart Name = {\
	.configuration = &Name##_config,\
};



#define _USART_MACRO_H_
#endif