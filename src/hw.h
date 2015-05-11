#ifndef _HW_H_

#define VREF 3.295
#define R1 100000.0

#define NTC_R	150000
#define NTC_A1	3.354016E-03
#define NTC_B1	2.367720E-04
#define NTC_C1	3.585140E-06
#define NTC_D1	1.255349E-07
#define T0_K	273.15

#define PWM_PERIOD 4096
#define PWM_TOP (PWM_PERIOD - 1)

void hw_init();

enum hw_init_state {
	HW_INIT_UNINITIALIZED,
	HW_INIT_RCC,
	HW_INIT_GPIO,
	HW_INIT_PRE_NVIC,
	HW_INIT_NVIC,
	HW_INIT_POST_INIT,
	HW_INIT_INITIALIZED,
};


void hw_init_state(enum hw_init_state state);


#define _HW_H_
#endif