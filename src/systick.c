#include "systick.h"

void sys_tick_handler(void) {
	if (ms_time_delay) {
		ms_time_delay--;
	}

	if (cooloff_timer != -1) {
		cooloff_timer++;
	}
	
	if (++mscounter == 1000) {
		mscounter = 0;
		system_time++;
		
	}

	adc_sample();

}

void sleep_ms(int t) {
	ms_time_delay = t;	while (ms_time_delay);
}


void init_systick() {
	ms_time_delay=0;
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(24000-1);  // 1 kHz
	systick_interrupt_enable();
	systick_counter_enable();


}
