/*! \file state_validate.c
    \brief Sensor validation state
 
*/
#include "irrigation.h"

/*! We set up the validation timer and init the
maximum and minim temperatured. We also check that we have current 
sensor data availble */
void state_validate_enter(struct irrigation_controller* controller) {
	controller->status.validation_timer = 5;
	//If data is too old we have a problem
	if (time_delta(controller->status.iteration_time, controller->status.soil_temperature.timestamp) < 0.01) {
		controller->status.min_temperature = controller->status.soil_temperature.temperature;
		controller->status.max_temperature = controller->status.soil_temperature.temperature;
	} else {
		controller->events.report_sensor_malfunction(controller);
		controller->pending_state = irrigation_status_REBOOT;
	}
}


/*! We have the heart beat LED status and we update the minimum and maximum values
while we continue to sample sensor data. When timeout is reached we verify that
the sensor data span is reasonable */
void state_validate_run(struct irrigation_controller* controller) {

	if (time_delta(controller->status.iteration_time, controller->status.soil_temperature.timestamp) > 0.01) {
		controller->events.report_sensor_malfunction(controller);
		controller->pending_state = irrigation_status_REBOOT;
	}


	math_ramp(h, controller->status.validation_timer*1.5);

	if (h <= 0.2) {
		h*=5;
	} else {
		h=0;
	}

	math_ramp(h1, h);
	math_ramp(h2, h+.2);
	float d = h1*h2;
	status_color(10.0*controller->status.validation_timer*d, 100.0*d, 0);


	controller->status.validation_timer -= controller->status.dt;

	controller->status.min_temperature = MIN(controller->status.min_temperature, controller->status.soil_temperature.temperature);
	controller->status.max_temperature = MAX(controller->status.max_temperature, controller->status.soil_temperature.temperature);

	if (controller->status.validation_timer <= 0) {
	
		controller->events.report_validation_temperatures(controller);
		if (controller->status.max_temperature - controller->status.min_temperature < 0.5) {
			controller->pending_state = irrigation_status_MEASURE;
		} else {
			controller->events.report_sensor_fluctuations(controller);
			controller->pending_state = irrigation_status_REBOOT;
		}
	}
}

/*! Nothing has to be done when we exit this state */
void state_validate_exit(struct irrigation_controller* controller) {
	
}