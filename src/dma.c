#include "dma.h"

void dma1_transmit_8_32(uint32_t src, uint32_t dst, uint32_t length, uint32_t channel) {

	dma_disable_channel(DMA1, channel);
	dma_channel_reset(DMA1, channel);
	dma_set_peripheral_address(DMA1, channel, dst);
	dma_set_memory_address(DMA1, channel, src);
	dma_set_number_of_data(DMA1, channel, length);
	dma_set_read_from_memory(DMA1, channel);
	dma_enable_memory_increment_mode(DMA1, channel);
	dma_set_peripheral_size(DMA1, channel, DMA_CCR_PSIZE_32BIT);
	dma_set_memory_size(DMA1, channel, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, channel, DMA_CCR_PL_VERY_HIGH);
	dma_enable_channel(DMA1, channel);

}
