#include "adc.h"

volatile static int adc_ready;
volatile static int ms_time_delay;

volatile static uint16_t* adc_samples[1];
volatile static int current_temperature=-1;


void adc_sample() {

	if (adc_ready) {

		dma_channel_reset(DMA1, DMA_CHANNEL1);

		dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t)&ADC1_DR);
		dma_set_memory_address(DMA1, DMA_CHANNEL1,(uint32_t)adc_samples);
		dma_set_number_of_data(DMA1, DMA_CHANNEL1, 1);
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
}


void init_adc() {
	adc_off(ADC1);

	adc_enable_scan_mode(ADC1);	
	adc_set_single_conversion_mode(ADC1);

	adc_set_right_aligned(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC);
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
	adc_enable_dma(ADC1);
	
	adc_power_on(ADC1);


	sleep_ms(2);	//Sleeping 1 ms may be less than one ms if systick is about to happen, 
	// we actually just need to wait 3µS according to http://libopencm3.github.io/docs/latest/stm32f1/html/group__adc__file.html#ga51f01f6dedbcfc4231e0fc1d8943d956

	adc_reset_calibration(ADC1);
	adc_calibration(ADC1);

	uint8_t channel_array[1] = {3};
	
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_ready = 1;
}


