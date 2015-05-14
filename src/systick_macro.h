#ifndef _SYSTICK_MACRO_H_

#include "systick.h"


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