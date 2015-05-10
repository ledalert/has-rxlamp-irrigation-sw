struct sample_range {
	int min;
	int max;
}

struct sample_buffer {
	int* buffer;
	int size;								//Size of the buffer
	int pos;								//Current position of the buffer
	int count;								//Count of samples in buffer, used to determine if data is filtered yet
	int sqsum;								//Total square sum of samples
	int invalid_samples_streak;				//Used for detecting sensor malfunction
	struct sample_range valid_range;
};

int buffer_add_sample(struct sample_buffer* buffer, int sample) {
	int result = -1;
	if ((sample >= buffer->valid_range.min) && (sample <= buffer->valid_range.max)) {

		buffer->sqsum -= buffer->buffer[buffer->pos];
		buffer->pos = (buffer->pos + 1) % buffer->size;
		if (buffer->count < buffer->size) {
			buffer->size++;
		}
		buffer->buffer[buffer->pos] = sample * sample;
		buffer->sqsum += buffer->buffer[buffer->pos];

		if (buffer->count == buffer->size) {
			result = buffer->sqsum / buffer->size;
		}

		invalid_samples_streak = 0;

	} else {
		invalid_samples_streak++;
	}
	return result;
}



