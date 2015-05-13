/*! \file state_reboot.c
    \brief Rebooting state
 
*/
#include "irrigation.h"



void state_reboot_enter(struct irrigation_controller* controller) {
	controller->status.initial_timer = 6;
}

void state_reboot_run(struct irrigation_controller* controller) {

	math_ramp(d, controller->status.initial_timer*5);
	status_color(255.0*d, 0, 0);


	controller->status.initial_timer -= controller->status.dt;

	if (controller->status.initial_timer <= 0) {
		controller->pending_state = irrigation_status_INIT;
	}
}

void state_reboot_exit(struct irrigation_controller* controller) {
	
}