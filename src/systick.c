/*! \file systick.c
    \brief Systick HAL implementation
 
*/
#include "systick.h"
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>


void systick_init(struct systick* systick, enum hw_init_state state) {
	if (state == HW_INIT_PRE_NVIC) {
		systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
		systick_set_reload(rcc_apb1_frequency / systick->configuration->frequency - 1);
	}

	if (state == HW_INIT_NVIC) {
		systick_interrupt_enable();		
	}

	if (state == HW_INIT_POST_INIT) {
		systick_counter_enable();

	}
}