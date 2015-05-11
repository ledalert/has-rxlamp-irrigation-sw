#include "moisture_sensor.h"
volatile int cooloff_timer = -1;


volatile int sample_data[250];
volatile struct sample_buffer ntc_resistor = {
	.buffer = sample_data,
	.size = 250,
	.pos = 0,
	.count = 0,
	.sqsum = 0,
	.invalid_samples_streak = 0,
	.valid_range = {
		// .min = 1755,
		// .max = 3290,
		.min = 0,
		.max = 4095,
	},
};
