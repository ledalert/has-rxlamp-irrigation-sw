/*! \file state_measure.c
    \brief Measurement state
 
*/
#include "irrigation.h"

/*! Measure state enter

	The following things are done in this function	

 */
void state_measure_enter(struct irrigation_controller* controller) {


	/*! @li It is asserted that the last soil temperature is no older than 10 mS */
	if (time_delta(controller->status.iteration_time, controller->status.soil_temperature.timestamp) > 0.01) {
		controller->events.report_sensor_malfunction(controller);
		controller->pending_state = irrigation_status_REBOOT;
	}

	/*! @li last_soil_sample is set to soil_temperature */
	controller->status.last_soil_sample = controller->status.soil_temperature;

	/*! @li Initializing variables used in run state */
	controller->status.heating_time=0;
	controller->status.sensor_state = moisture_sensor_heating;	
	controller->status.periodic_temp_report = 0;
	controller->status.ackumulative_delta_temp = 0;
	controller->status.ackumulative_delta_time = 0;

	// controller->status.validation_timer = 5;
	// if (controller->status.soil_temperature.timestamp.ms == controller->status.iteration_time.ms && controller->status.soil_temperature.timestamp.epoch == controller->status.iteration_time.epoch) {		
	// 	controller->status.min_temperature = controller->status.soil_temperature.temperature;
	// 	controller->status.max_temperature = controller->status.soil_temperature.temperature;
	// } else {
	// 	controller->events.report_sensor_malfunction(controller);
	// 	controller->pending_state = irrigation_status_REBOOT;
	// }

	/*! @li Report that heater was started */
	controller->events.report_msg_note(controller, "Heater started");

}



void state_measure_run(struct irrigation_controller* controller) {


	if (time_delta(controller->status.iteration_time, controller->status.soil_temperature.timestamp) > 0.01) {
		controller->events.report_sensor_malfunction(controller);
		controller->pending_state = irrigation_status_REBOOT;
	}


	float delta_temp_time = time_delta(controller->status.soil_temperature.timestamp, controller->status.last_soil_sample.timestamp);
	float delta_temp_temp = controller->status.soil_temperature.temperature - controller->status.last_soil_sample.temperature;

	controller->status.last_soil_sample = controller->status.soil_temperature;
	controller->status.ackumulative_delta_temp += fabs(delta_temp_temp);
	controller->status.ackumulative_delta_time += delta_temp_time;

	

	controller->status.periodic_temp_report += controller->status.dt;
	if (controller->status.periodic_temp_report > 1.0) {
		controller->status.periodic_temp_report = 0;
		controller->events.report_current_temperature(controller);
	}

	if (controller->status.sensor_state == moisture_sensor_heating) {
		controller->status.heating_time += controller->status.dt;
		if (controller->status.heating_time > 10) {
			controller->events.report_msg_note(controller, "Heater stopped");
			controller->heater = 0;
			controller->status.last_hot_sample = controller->status.soil_temperature;
			controller->status.sensor_state = moisture_sensor_cooling;
			controller->status.cooling_time = 0;
		} else {
			controller->heater = MIN(controller->status.heating_time * 4095.0, 4095);
			math_ramp(h, controller->status.heating_time/10.0);
			status_color(h*200+55, 0,20+ h*90);
		}
	} else 	if (controller->status.sensor_state == moisture_sensor_cooling) {
		controller->status.cooling_time += controller->status.dt;
		if (controller->status.cooling_time > 60) {		//Cooling took too long
			

			controller->events.report_measurement_data(controller);
			controller->pending_state = irrigation_status_REBOOT;

		} else {				
			math_ramp(h, controller->status.cooling_time/60.0);
			h= 1.0 - h;
			status_color(h*90+20, 0, h*200+55);
		}


	}

	// math_ramp(h, controller->status.measure_timer*1.5);

	// if (h <= 0.2) {
	// 	h*=5;
	// } else {
	// 	h=0;
	// }

	// math_ramp(h1, h);
	// math_ramp(h2, h+.2);
	// float d = h1*h2;
	// status_color(10.0*controller->status.validation_timer*d, 100.0*d, 0);


	// controller->status.validation_timer -= controller->status.dt;

	// controller->status.min_temperature = MIN(controller->status.min_temperature, controller->status.soil_temperature.temperature);
	// controller->status.max_temperature = MAX(controller->status.max_temperature, controller->status.soil_temperature.temperature);

	// if (controller->status.validation_timer <= 0) {
	
	// 	controller->events.report_validation_temperatures(controller);
	// 	if (controller->status.max_temperature - controller->status.min_temperature < 0.5) {
	// 		controller->pending_state = irrigation_status_MEASURE;
	// 	} else {
	// 		controller->events.report_sensor_malfunction(controller);
	// 		controller->pending_state = irrigation_status_REBOOT;
	// 	}
	// }
}

void state_measure_exit(struct irrigation_controller* controller) {
	
}