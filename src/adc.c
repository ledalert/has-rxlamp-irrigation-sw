/*! \file adc.c
    \brief ADC HAL implementation
 
*/
#include "adc.h"
#include "timer.h"
#include <libopencm3/cm3/nvic.h>


//Todo handle more params, such as dma =)

static struct adc* adc1 = 0;

void adc_init(struct adc* adc, enum hw_init_state state) {

	adc1 = adc;

	if (state == HW_INIT_RCC) {
		rcc_periph_clock_enable(RCC_ADC1);
	}


	if (state == HW_INIT_PRE_NVIC) {
		adc_off(ADC1);

		adc_enable_scan_mode(ADC1);	
		adc_set_single_conversion_mode(ADC1);

		adc_set_right_aligned(ADC1);
		adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC);
		adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
		
		adc_power_on(ADC1);


		int delay = rcc_apb1_frequency * 3 / 2000000;
		asm volatile (	//I think this one takes 2 clock cycles per iteration, not verified though
 			"loop: subs r3, #1;"
            "bne loop;"
            :: "r" (delay)
		);


		adc_reset_calibration(ADC1);
		adc_calibration(ADC1);		

		adc_set_regular_sequence(ADC1, adc->configuration->channel_count, adc->configuration->channel_map);
		adc_enable_dma(ADC1);
	}

	if (state == HW_INIT_NVIC) {
		nvic_set_priority(NVIC_DMA1_CHANNEL1_IRQ, 0);
		nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

	}

}

void adc_begin_sampling(struct adc* adc) {

	dma_channel_reset(DMA1, DMA_CHANNEL1);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t)&ADC1_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL1,(uint32_t)adc->adc_buffer);
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, adc->configuration->channel_count);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
	dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
	dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_VERY_HIGH);

	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);

	dma_enable_channel(DMA1, DMA_CHANNEL1);

	adc_start_conversion_regular(ADC1);

}


