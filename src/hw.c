/*! \file hw.c
    \brief HAL main control
 
*/
#include "hw.h"
// #include "pump.h"
// #include "moisture_sensor.h"
//#include "led.h"
// #include "uart.h"

// #include <libopencm3/stm32/timer.h>
// #include <libopencm3/stm32/rcc.h>
// #include <libopencm3/cm3/nvic.h>



void hw_init() {
	hw_init_state(HW_INIT_RCC);
	hw_init_state(HW_INIT_GPIO);
	hw_init_state(HW_INIT_PRE_NVIC);
	hw_init_state(HW_INIT_NVIC);
	hw_init_state(HW_INIT_POST_INIT);
}

