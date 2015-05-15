/*! \file systick_macro.h
    \brief Helper macros for systick HAL
 
*/
#ifndef _SYSTICK_MACRO_H_

#include "systick.h"

/*! Automatic systick set to frequency, autostarts */
#define SYSTICK_AUTO_CONFIG(Name, Frequency)\
	struct systick_config Name##_config = {\
		.frequency = Frequency,\
		.auto_start = 1,\
	};\
	\
	struct systick Name = {\
		.configuration = &Name##_config,\
	};


#define _SYSTICK_MACRO_H_
#endif