/*! \file dma.c
    \brief DMA implementation
 
*/
#include "dma.h"


void dma_init(struct dma_config* dma, enum hw_init_state state) {
	
	if (state == HW_INIT_RCC) {
		rcc_periph_clock_enable(dma->rcc);
	}
}


void dma_channel_init(struct dma_channel_config* channel, enum hw_init_state state) {

	dma_init(channel->dma, state);

	if (state==HW_INIT_POST_INIT) {

		dma_disable_channel(channel->dma->dma, channel->channel);
		dma_channel_reset(channel->dma->dma, channel->channel);
		dma_set_peripheral_address(channel->dma->dma, channel->channel, (uint32_t) channel->perhipheral);
		dma_set_read_from_memory(channel->dma->dma, channel->channel);
		dma_enable_memory_increment_mode(channel->dma->dma, channel->channel);
		dma_set_peripheral_size(channel->dma->dma, channel->channel, DMA_CCR_PSIZE_32BIT);
		dma_set_memory_size(channel->dma->dma, channel->channel, DMA_CCR_MSIZE_8BIT);
		dma_set_priority(channel->dma->dma, channel->channel, DMA_CCR_PL_VERY_HIGH);
		dma_enable_channel(channel->dma->dma, channel->channel);
	}

}

void dma_channel_send(struct dma_channel_config* channel, volatile void* data, int length) {


	*channel->enable_reg &= ~channel->enable_flag;
	dma_disable_channel(channel->dma->dma, channel->channel);

	dma_set_memory_address(channel->dma->dma, channel->channel, (uint32_t) data);
	dma_set_number_of_data(channel->dma->dma, channel->channel, length);

	dma_enable_channel(channel->dma->dma, channel->channel);
	*channel->enable_reg |= channel->enable_flag;


}

void dma_channel_send_blocking(struct dma_channel_config* channel, volatile void* data, int length) {
	dma_channel_send(channel, data, length);
	while (!dma_get_interrupt_flag(channel->dma->dma, channel->channel, DMA_TCIF));

}
