#include "filter.h"
int buffer_add_sample(volatile struct sample_buffer* data, int sample) {
	int result = -1;
	if ((sample >= data->valid_range.min) && (sample <= data->valid_range.max)) {

		if (data->count < data->size) {
			data->count++;

		} else {
			data->sqsum -= data->buffer[data->pos];	//Remove tail from result (pos is both head and tail)
		}

		data->buffer[data->pos] = sample * sample;
		data->sqsum += data->buffer[data->pos];	//Add square of current sample to sum

		if (data->count == data->size) {
			result = data->sqsum;			//Return square sum
		}

		data->invalid_samples_streak = 0;
		data->pos = (data->pos + 1) % data->size; //advance position

	} else {
		data->invalid_samples_streak++;
	}
	return result;
}


void dma1_channel1_isr(void) {
	DMA1_IFCR |= DMA_IFCR_CTCIF1;
	current_temperature = buffer_add_sample(&ntc_resistor, (int)adc_samples[0]);
}
