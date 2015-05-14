/*! \file timer.h
    \brief Timer HAL include file
 
*/
#ifndef _TIMER_H_

#include "hw.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include "dma.h"

struct timer;

struct timer_ccr_config {
	struct timer* timer;
	enum tim_oc_id channel;
	struct dma_channel_config* dma;
	volatile uint32_t* reg;

};

struct timer_ccr {
	struct timer_ccr_config* configuration;
	enum hw_init_state state;
	uint32_t ccr;
};


struct timer_config {
	uint32_t timer;
	uint32_t rcc;
};

struct timer {
	struct timer_config* configuration;
	enum hw_init_state state;
	uint32_t auto_reload;
};


void timer_ccr_init(struct timer_ccr* ccr, enum hw_init_state state);
void timer_init(struct timer* timer, enum hw_init_state state);
void timer_ccr_set(struct timer_ccr* ccr, uint32_t value);

#define _TIMER_H_
#endif