#ifndef _FILTER_H_

struct sample_range {
	int min;
	int max;
};

struct sample_buffer {
	volatile int* buffer;
	int size;								//Size of the buffer
	int pos;								//Current position of the buffer
	int count;								//Count of samples in buffer, used to determine if data is filtered yet
	int sqsum;								//Total square sum of samples
	int invalid_samples_streak;				//Used for detecting sensor malfunction
	struct sample_range valid_range;
};



#define _FILTER_H_
#endif