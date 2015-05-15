/*! \file timer_macro.h
    \brief Utility macros for timers
 
*/
#ifndef _TIMER_MACRO_H_
#include "timer.h"


/*! Create timer instance */
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


/*! Create timer CRR channel instance */
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



/*! Create unconfigured timer */
#define TIMER_NOCONF(Name, Timer, Rcc) TIMER_INSTANCE(Name, Timer, Rcc, 0)
/*! Create unconfigured timer CCR channel */
#define TIMER_CCR_NOCONF(Name, Timer, Channel, Dma) TIMER_CCR_INSTANCE(Name, Timer, Channel, Dma, 0)


#define _TIMER_MACRO_H_
#endif