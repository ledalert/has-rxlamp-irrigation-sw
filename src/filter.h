/*! \file filter.h
    \brief Filter functions for filtering sensor data
 
*/
#ifndef _FILTER_H_
#include <stdint.h>

/*! A valid range for samples */
struct filter_sample_range {
	int min;
	int max;
};

/*! RMS filter configuration and state */
struct filter_rms {
	volatile uint32_t* buffer;
	uint32_t size;								/*!< Size of the buffer */
	int pos;									/*!< Current position of the buffer */
	uint32_t count;								/*!< Count of samples in buffer, used to determine if data is filtered yet */
	uint32_t sqsum;								/*!< Total square sum of samples @note If you use 12 bit samples, the square take up to 24 bits, this means you get 8 bits left, so max filter length is 256!*/
	int invalid_samples_streak;					/*!< Used for detecting sensor malfunction */
	struct filter_sample_range valid_range;
};

int filter_rms_process(volatile struct filter_rms* data, int sample);

#define _FILTER_H_
#endif