#ifndef _SYSTICK_H_
#include "hw.h"
#include <stdint.h>
#include <stdbool.h>

struct systick_config {
	uint32_t frequency;
	bool auto_start;
};

struct systick {
	struct systick_config* configuration;
};

void systick_init(struct systick* systick, enum hw_init_state state);

#define _SYSTICK_H_
#endif