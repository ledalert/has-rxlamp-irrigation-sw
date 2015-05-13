#ifndef _TIMER_MACRO_H_
#include "timer.h"

#define TIMER1_CCR3N_DMA TIM1, RCC_TIM1, RCC_DMA1, TIM_OC3N, &TIM1_CCR3, DMA_CHANNEL6, TIM_DIER_CC3DE
#define TIMER2_CCR3 TIM2, RCC_TIM2, TIM_OC3, &TIM2_CCR3
#define TIMER3_CCR3 TIM3, RCC_TIM3, TIM_OC3, &TIM3_CCR3

#define TIMER_CCR_INSTANCE(...) TIMER_CCR_INSTANCE_EXP(__VA_ARGS__)

//TIMER_CCR_INSTANCE(Pump, &GPIO_PB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0, TIMER3_CCR3, 4095, 0);


//TIMER_CCR_INSTANCE(         Pump, &GPIO_PB,GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0, TIM3, RCC_TIM3, TIM_OC3, &TIM3_CCR3, 4095, 0);
#define TIMER_CCR_INSTANCE_EXP(Name, Port, Mode, Configuration, Pin, Timer, Timer_rcc, Timer_rcc_channel, Timer_rcc_register, Timer_period, Timer_rcc_start)\
\
struct gpio_pin_config Name##_pin_config = {\
	.port = Port,\
	.pin = Pin,\
	.mode = Mode,\
	.configuration = Configuration,\
};\
\
struct gpio_pin Name##_pin = {\
	.configuration = &Name##_pin_config,	\
};\
\
struct timer_config Name##_timer_config = {\
	.timer = Timer,\
	.rcc = Timer_rcc,\
	.auto_reload = Timer_period,\
};\
\
struct timer Name##_timer = {\
	.configuration = &Name##_timer_config,	\
};\
\
struct timer_ccr_config Name##_config = {\
	.timer = &Name##_timer,\
	.channel = Timer_rcc_channel,\
	.start_ccr = Timer_rcc_start,\
	.reg = Timer_rcc_register,\
};\
\
struct timer_ccr Name = {\
	.configuration = &Name##_config,\
};


#define _TIMER_MACRO_H_
#endif