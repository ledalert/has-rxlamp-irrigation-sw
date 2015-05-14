/*! \file timer_macro.h
    \brief Utility macros for timers
 
*/
#ifndef _TIMER_MACRO_H_
#include "timer.h"

//TIMER_INSTANCE(StatusLED_timer, TIM1, RCC_TIM1, &StatusLED_DMA);


#define TIMER_INSTANCE(Name, Timer, Rcc, Period)\
    struct timer_config Name##_config = {\
    	.timer = Timer,\
    	.rcc = Rcc,\
    };\
	\
    struct timer Name = {\
    	.configuration = &Name##_config,\
    	.auto_reload = Period,\
    };


//TIMER_CCR_INSTANCE(StatusLED_pwm, &StatusLED_timer, TIM_OC3N)
//StatusLED_pwm, &StatusLED_timer, TIM_OC3N, &StatusLED_dma, 0
#define TIMER_CCR_INSTANCE(Name, Timer, Channel, Dma, StartCCR)\
    struct timer_ccr_config Name##_config = {\
    	.timer = Timer,\
    	.channel = Channel,\
    	.dma = Dma,\
    };\
    struct timer_ccr Name = {\
    	.configuration = &Name##_config,\
    	.ccr = StartCCR,\
	};




#define TIMER_NOCONF(Name, Timer, Rcc) TIMER_INSTANCE(Name, Timer, Rcc, 0)
#define TIMER_CCR_NOCONF(Name, Timer, Channel, Dma) TIMER_CCR_INSTANCE(Name, Timer, Channel, Dma, 0)


#define _TIMER_MACRO_H_
#endif