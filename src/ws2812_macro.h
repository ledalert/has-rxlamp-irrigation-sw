#ifndef _WS2812_MACRO_H_
#include "ws2812.h"

// #define GPIO_PB15 GPIOB, RCC_GPIOB, GPIO15
 

#define WS2812_INSTANCE(...) WS2812_INSTANCE_EXP(__VA_ARGS__)

                                   //TIM1,            RCC_TIM1,             RCC_DMA1,          TIM_OC3N,     &TIM1_CCR3,                     DMA_CHANNEL6,            TIM_DIER_CC3DE

#define WS2812_INSTANCE_EXP(Name, CCR_Channel_Timer, CCR_Channel_Timer_RCC, CCR_Timer_DMA_RCC, CCR_Channel, CCR_Channel_CCR_Register, CCR_Channel_DMA_Channel, CCR_Channel_DMA_Enable_Flag, Pin_Port, Pin_Port_RCC, Pin, LED_Count)\
\
struct gpio_port_config Name##_port_config = {\
	.port = Pin_Port,\
	.rcc = Pin_Port_RCC,\
};\
\
struct gpio_port Name##_port = {\
	.configuration = &Name##_port_config,\
};\
\
struct gpio_pin_config Name##_pin_config = {\
	.port = &Name##_port,\
	.pin = Pin,	\
};\
\
struct gpio_pin Name##_pin = {\
	.configuration = &Name##_pin_config,\
};\
\
struct timer_config Name##_timer_config = {\
	.timer = CCR_Channel_Timer,\
	.rcc = CCR_Channel_Timer_RCC,\
	.dma_rcc = CCR_Timer_DMA_RCC,\
};\
\
struct timer Name##_timer = {\
	.configuration = &Name##_timer_config,\
};\
\
struct timer_ccr_config Name##_timer_ccr_config = {\
	.timer = &Name##_timer,\
	.channel = CCR_Channel,\
	.reg = CCR_Channel_CCR_Register,\
	.dma_channel = CCR_Channel_DMA_Channel,\
	.dma_enable_flag = CCR_Channel_DMA_Enable_Flag,\
};\
\
struct timer_ccr Name##_timer_ccr = {\
	.configuration = &Name##_timer_ccr_config,\
};\
\
struct ws2812_config Name##_config = {\
	.ccr = &Name##_timer_ccr,\
	.pin = &Name##_pin,\
};\
\
struct ws2812_rgb Name##_buffer[LED_Count];\
uint8_t Name##_pwm_buffer[24 * LED_Count + 1];\
\
struct ws2812 Name = {\
	.configuration = &Name##_config,\
	.led_buffer = Name##_buffer,\
	.pwm_buffer = Name##_pwm_buffer,\
	.led_count = LED_Count,\
};


#define _WS2812_MACRO_H_
#endif
