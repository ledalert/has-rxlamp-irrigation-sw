/*! \file uart.h
    \brief UART HAL Include file
 
*/
#ifndef _UART_H_
#include "gpio.h"
#include "time.h"
#include <libopencm3/stm32/usart.h>

/*! Send string literal using blocking/polling (no dma or irq) */
#define usart_blocking_str(uart, str) usart_blocking_send(uart, str, sizeof(str)-1)

/*! Usart configuration */
struct usart_config {
	struct gpio_pin* rx_pin;
	struct gpio_pin* tx_pin;
	uint32_t usart;
	uint32_t rcc;

	uint32_t baudrate;
	uint32_t databits;
	uint32_t parity;
	uint32_t stopbits;
	uint32_t mode;
	uint32_t flowcontrol;
};

/*! Usart */
struct usart {
	struct usart_config* configuration;
};


void usart_init(struct usart* usart, enum hw_init_state state);
void usart_blocking_send(struct usart* usart, volatile void* data, int length);
void usart_blocking_float(struct usart* usart, float data);
void usart_blocking_int(struct usart* usart, int data);
void usart_blocking_int_zp(struct usart* usart, int data, int dp);
void usart_blocking_strz(struct usart* usart, volatile char* data);
void usart_blocking_tm(struct usart* usart, struct sw_timer_system_time* tm);
#define _UART_H_
#endif