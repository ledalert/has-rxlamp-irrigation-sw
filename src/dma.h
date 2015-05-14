/*! \file dma.h
    \brief DMA includes
 
*/
#ifndef _DMA_H_
#include "hw.h"

#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>


struct dma_config {
	uint32_t dma;
	uint32_t rcc;
};

struct dma_channel_config {
	struct dma_config* dma;
	uint32_t channel;
	volatile uint32_t* perhipheral;
	volatile uint32_t* enable_reg;
	uint32_t enable_flag;
};


void dma_init(struct dma_config* dma, enum hw_init_state state);
void dma_channel_init(struct dma_channel_config* channel, enum hw_init_state state);
void dma_channel_send(struct dma_channel_config* channel, volatile void* data, int length);
void dma_channel_send_blocking(struct dma_channel_config* channel, volatile void* data, int length);



#define _DMA_H_
#endif