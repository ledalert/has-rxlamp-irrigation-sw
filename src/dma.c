#include "dma.h"

void uart_send_data(volatile void* data, int length);
void DEBUG_PRINT_HEX(int value);
#define DEBUG_PRINT(msg) uart_send_data(msg, sizeof(msg)-1);


void dma1_transmit_8_32(uint32_t src, uint32_t dst, uint32_t length, uint32_t channel) {

	DEBUG_PRINT("Initializing DMA transfer from ");
	DEBUG_PRINT_HEX(src);
	DEBUG_PRINT(" to ");
	DEBUG_PRINT_HEX(dst);
	DEBUG_PRINT(".\nLength: ");
	DEBUG_PRINT_HEX(length);
	DEBUG_PRINT(" Channel: ");
	DEBUG_PRINT_HEX(channel);


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

	DEBUG_PRINT("DMA Enable");

	dma_enable_channel(DMA1, channel);

}
