#ifndef _TIMER_H_

#include "hw.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>


struct timer_ccr {
	struct timer_ccr_config* configuration;
	enum hw_init_state state;
};

struct timer_ccr_config {
	struct timer* timer;
	enum tim_oc_id channel;
	uint32_t initial_reload;
};

struct timer_config {
	uint32_t timer;
	uint32_t rcc;
	uint32_t initial_reload;
};

struct timer {
	struct timer_config* configuration;
	enum hw_init_state state;
	uint32_t reload;
};


void timer_ccr_init(struct timer_ccr* ccr, enum hw_init_state state);
void timer_init(struct timer* timer, enum hw_init_state state);

#define _TIMER_H_
#endif