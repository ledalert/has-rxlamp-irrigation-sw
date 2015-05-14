/*! \file filter.c
    \brief Implementation of filter functions
 
*/
#include "filter.h"

/*! Adds a sample to the filter
 	@param data Filter state
 	@param sample New sample
 	@returns Filtered result if possible, otherwise -1 */
int filter_rms_process(volatile struct filter_rms* data, int sample) {
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
			result = (int) (data->sqsum / data->size);			//Return square sum
		}

		data->invalid_samples_streak = 0;
		data->pos = (data->pos + 1) % data->size; //advance position

	} else {
		data->invalid_samples_streak++;
	}
	return result;
}

