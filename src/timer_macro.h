/*! \file timer_macro.h
    \brief Utility macros for timers
 
*/
#ifndef _TIMER_MACRO_H_
#include "timer.h"

#define TIMER1_CCR3N_DMA TIM1, RCC_TIM1, RCC_DMA1, TIM_OC3N, &TIM1_CCR3, DMA_CHANNEL6, TIM_DIER_CC3DE
#define TIMER2_CCR3 TIM2, RCC_TIM2, TIM_OC3, &TIM2_CCR3
#define TIMER3_CCR3 TIM3, RCC_TIM3, TIM_OC3, &TIM3_CCR3

/*! Wrapper for @ref TIMER_CCR_INSTANCE_EXP */
#define TIMER_CCR_INSTANCE(...) TIMER_CCR_INSTANCE_EXP(__VA_ARGS__)

//TIMER_CCR_INSTANCE(Pump, &GPIO_PB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0, TIMER3_CCR3, 4095, 0);


//TIMER_CCR_INSTANCE(         Pump, &GPIO_PB,GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0, TIM3, RCC_TIM3, TIM_OC3, &TIM3_CCR3, 4095, 0);
/*! Creates a few objects needed for a timer CCR channel
	
	@param Name	Prepends the following variables that are created by the macro
	@param Port What struct_port to use
	@param Mode Mode for GPIO pin
	@param Configuration Configuration for GPIO pin
	@param Timer Timer base address
	@param Timer_rcc Timer RCC (enum rcc_periph_clken)
	@param Timer_ccr_channel CCR channel of timer (enum tim_oc_id)
	@param Timer_ccr_register Register for timer CCR register
	@param Timer_period Auto reload register value
	@param Timer_ccr_start Initial CCR value


	@returns

	@li struct gpio_pin_config {Name}_pin_config
	@li struct gpio_pin {Name}_pin
	@li struct timer_config {Name}_timer_config
	@li struct timer {Name}_timer
	@li struct timer_ccr_config {Name}_config
	@li struct timer_ccr {Name}


 */
#define TIMER_CCR_INSTANCE_EXP(Name, Port, Mode, Configuration, Pin, Timer, Timer_rcc, Timer_ccr_channel, Timer_ccr_register, Timer_period, Timer_ccr_start)\
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
	.channel = Timer_ccr_channel,\
	.start_ccr = Timer_ccr_start,\
	.reg = Timer_ccr_register,\
};\
\
struct timer_ccr Name = {\
	.configuration = &Name##_config,\
};


#define _TIMER_MACRO_H_
#endif