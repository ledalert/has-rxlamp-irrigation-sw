/*! \file state_init.c
    \brief Initial state
 
*/
#include "irrigation.h"

/*! Reset all variables in the controller this state uses */
void state_init_enter(struct irrigation_controller* controller) {
	controller->status.initial_timer = 6;
}

/*! We will make a heart beat ramp for the status LED while 
	we wait for initial delay to complete */
void state_init_run(struct irrigation_controller* controller) {

	math_ramp(h, controller->status.initial_timer);

	if (h <= 0.2) {
		h*=5;
	} else {
		h=0;
	}

	math_ramp(h1, h);
	math_ramp(h2, h+.2);
	float d = h1*h2;
	status_color(10.0*controller->status.initial_timer*d, 100.0*d, 0);

	controller->status.initial_timer -= controller->status.dt;


	if (controller->status.initial_timer <= 0) {
		controller->pending_state = irrigation_status_VALIDATE;
	}
}

/*! Nothing has do be done before we leave this state */
void state_init_exit(struct irrigation_controller* controller) {
	
}