/*! \file irrigation.h
    \brief Irrigation main include file
 
*/

#ifndef _IRRIGATION_H_

#include "time.h"
#include "math.h"

#define time_delta(a, b) ((float)(a.epoch - b.epoch) + (float)(a.ms - b.ms) / 1000.0)	
/*!< @brief Calculates the time difference in seconds between two timestamps
	
	@param a	Time event A
	@param b	Time event B

	@returns	Returns a - b

 */

/*! This is the state of the irrigation controller */
enum irrigation_status {
	irrigation_status_REBOOT = -2,			/*!< If something goes terribly wrong we can set this to pending state and force a reboot */
	irrigation_status_UNDEFINED = -1,		/*!< This is the default state but it is also used for the pending state when no pending state is set */

	irrigation_status_INIT,					/*!< Initialize the system */
	irrigation_status_VALIDATE,				/*!< Validate the sensors */
	irrigation_status_MEASURE,				/*!< Measure soil water content */

};

/*! This is a timestamped temperature sample */
struct irrigation_temperature_sample {
	struct sw_timer_system_time timestamp;	/*!< Time sample was taken */
	float temperature;						/*!< Temperature */
};

/*! This is the state of the moisture sensor */
enum moisture_sensor_state {
	moisture_sensor_heating,	/*!< The sensor is heating up */
	moisture_sensor_cooling,	/*!< The sensor is cooling down */
	moisture_sensor_t1,			/*!< @todo t1 and t2 are not used for the moment */
	moisture_sensor_t2,
};

struct irrigation_controller_status {
	float dt;													/*!< Delta time since last state run */
	struct sw_timer_system_time iteration_time;					/*!< Timestamp of when this run started */
	struct sw_timer_system_time previous_iteration_time;		/*!< Timestamp of previous run, used to calculate @ref dt */
	enum moisture_sensor_state sensor_state;
	float ackumulative_delta_temp;
	float ackumulative_delta_time;
	float heating_time;
	float cooling_time;
	struct irrigation_temperature_sample soil_temperature;		/*!< Last soil temperature measured */
	struct irrigation_temperature_sample last_soil_sample;		/*!< Holds the temperature of an initial water content sampling */
	struct irrigation_temperature_sample last_hot_sample;		/*!< @todo Clean up unused stuff here */
	struct irrigation_temperature_sample last_t1_sample;
	struct irrigation_temperature_sample last_t2_sample;
	float min_temperature;										/*!< This is the minimum temperature during @ref state_validate */
	float max_temperature;										/*!< This is the minimum temperature during @ref state_validate */
	float validation_timer;										/*!< This is the timer for @ref state_validate */
	float initial_timer;										/*!< This is the timer for @ref state_init */
	float periodic_temp_report;									/*!< This is a timer for periodic calls to @ref irrigation_events::report_current_temperature */
};

struct irrigation_controller;

/*! The event callback functions for the irrigation controller core  */
struct irrigation_events {
	void (*report_validation_temperatures)(struct irrigation_controller*);	/*!< Report the measured temperatures for validation @sa state_validate_enter min_temperature max_temperature*/
	void (*report_sensor_malfunction)(struct irrigation_controller*);		/*!< Report that sensordata is invalid */
	void (*report_sensor_fluctuations)(struct irrigation_controller*);		/*!< Report that sensordata fluctuates */
	void (*report_current_temperature)(struct irrigation_controller*);		/*!< Reports current temperature */
	void (*report_measurement_data)(struct irrigation_controller*);			/*!< Report water content data */
	void (*report_msg_note)(struct irrigation_controller*, char*);		/*!< Generic note as a string, used for debugging */
};



/*! Holds everything related to the irrigation controller */
struct irrigation_controller {
	struct irrigation_controller_status status;	/*!< Status and measurements */
	enum irrigation_status current_state;		/*!< Current state in the @ref state_machine */
	enum irrigation_status pending_state;		/*!< Pending state in the @ref state_machine */
	struct irrigation_events events;
	int heater;
	int pump;
};


/** @defgroup state_init Initial state **/
/** @{ */
void state_init_enter(struct irrigation_controller* controller);
void state_init_run(struct irrigation_controller* controller);
void state_init_exit(struct irrigation_controller* controller);
/** @} */

/** @defgroup state_validate Sensor validation state */
/** @{ */
void state_validate_enter(struct irrigation_controller* controller);
void state_validate_run(struct irrigation_controller* controller);
void state_validate_exit(struct irrigation_controller* controller);
/** @} */

/** @defgroup state_reboot Reboot state
This state makes sure the systems exits in a nice way before it starts up again */

/** @{ */
void state_reboot_enter(struct irrigation_controller* controller);
void state_reboot_run(struct irrigation_controller* controller);
void state_reboot_exit(struct irrigation_controller* controller);
/** @} */

/** @defgroup state_measure This state makes the actual soil measure */
/** @{ */
void state_measure_enter(struct irrigation_controller* controller);
void state_measure_run(struct irrigation_controller* controller);
void state_measure_exit(struct irrigation_controller* controller);
/** @} */ 


void status_color(int r, int g, int b);

#define _IRRIGATION_H_
#endif