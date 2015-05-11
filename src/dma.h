#ifndef _DMA_H_

#include <libopencm3/stm32/dma.h>


void dma1_transmit_8_32(uint32_t src, uint32_t dst, uint32_t length, uint32_t channel);


#define _DMA_H_
#endif