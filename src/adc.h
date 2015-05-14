/*! \file adc.h
    \brief ADC HAL include
 
*/
#ifndef _ADC_H_
#include "hw.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>



struct adc_config {
	uint8_t* channel_map;
	int channel_count;
};

struct adc {
	struct adc_config* configuration;
	uint16_t* adc_buffer;
	bool sampling_done;
};


void adc_init(struct adc* adc, enum hw_init_state state);
void adc_begin_sampling(struct adc* adc);

#define _ADC_H_
#endif