/*! \file main.c
    \brief Irrigation controller main file
 
 	@todo	systick, adc, moisture sensor, pwm output, filter
 	@todo   explain port and rcc better

	@mainpage Irrigation Controller
	@section index_sec Index

	@li main.c Main file


*/

#include "math.h"
#include "ws2812_macro.h"
#include "usart_macro.h"

#include "led_macro.h"
#include "timer_macro.h"
#include "systick.h"
#include "adc.h"
#include "time.h"
#include "filter.h"
#include "irrigation.h"

//Create hardware configuration
GPIO_PORT_INSTANCE(GPIO_PA, GPIOA, RCC_GPIOA);
GPIO_PORT_INSTANCE(GPIO_PB, GPIOB, RCC_GPIOB);
WS2812_INSTANCE(StatusLED, TIMER1_CCR3N_DMA, &GPIO_PB, GPIO15, 1);
USART_TXONLY_INSTANCE(Serial, USART1, RCC_USART1, 921600, 0, 0, 0, 0, &GPIO_PA, GPIO_USART1_TX);

TIMER_CCR_INSTANCE(Pump, &GPIO_PB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0, TIMER3_CCR3, 4095, 0);
TIMER_CCR_INSTANCE(Heater, &GPIO_PA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2, TIMER2_CCR3, 4095, 0);

GAMMA_LUT_8_8_E_INSTANCE(LED_Gamma);


GPIO_PIN_INSTANCE(NTC_pin, &GPIO_PA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO3);

struct systick_config Systick_config = {
	.frequency = 1000,
	.auto_start = 1,
};

struct systick Systick = {
	.configuration = &Systick_config,
};


struct adc_config NTC_adc_config = {
	.channel_map = &(uint8_t){3},
	.channel_count = 1,
};

uint16_t NTC_buffer[1];

struct adc NTC_adc = {
	.configuration = &NTC_adc_config,
	.adc_buffer = NTC_buffer,
};


#define NTC_Filter_Size	250

volatile uint32_t NTC_filter_buffer[NTC_Filter_Size];

struct filter_rms NTC_filter = {
	.buffer = NTC_filter_buffer,
	.size = NTC_Filter_Size,
	.valid_range = {	//Only accept temperatures between 1..50 °C
		.min = 1200,
		.max = 3400,
	},
};



volatile int sleep_ms = 0;
volatile int NTC_filtered_value = -1;

void hw_init_state(enum hw_init_state state) {
	//Init hardware
	ws2812_init(&StatusLED, state);
	usart_init(&Serial, state);

	gpio_pin_init(&Pump_pin, state);
	gpio_pin_init(&Heater_pin, state);
	gpio_pin_init(&NTC_pin, state);

	timer_ccr_init(&Pump, state);
	timer_ccr_init(&Heater, state);

	systick_init(&Systick, state);
	adc_init(&NTC_adc, state);
}



volatile struct sw_timer_system_time system_time = {1420070400, 0};
volatile bool system_time_updated=0;


void sys_tick_handler(void) {
	if (sleep_ms) {
		sleep_ms--;
	}

	if (++system_time.ms == 1000) {	
		system_time.epoch++;
		system_time.ms=0;
	}
	system_time_updated = 1;

	adc_begin_sampling(&NTC_adc);

}


void get_system_time_blocking(struct sw_timer_system_time* result) {
	//Try to copy time but if it was updated, discard and retry
	while (1) {
		system_time_updated = 0;
		*result = system_time;			
		if (system_time_updated == 0) {
			break;
		}
	}
}


void dma1_channel1_isr(void) {
	DMA1_IFCR |= DMA_IFCR_CTCIF1;
	NTC_adc.sampling_done = 1;
	NTC_filtered_value = filter_rms_process(&NTC_filter, NTC_buffer[0]);
}


// enum irrigation_system_status {

// 	IRRIGATION_FAULT_SENSOR,
// 	IRRIGATION_STATUS_INIT,
// 	IRRIGATION_STATUS_SENSOR_VALID,
// 	IRRIGATION_STATUS_SENSOR_STABILIZED,

// 	IRRIGATION_STATUS_MEASURE_START,
// 	IRRIGATION_STATUS_MEASURE_HOT,
// 	IRRIGATION_STATUS_MEASURE_COMPLETE,


// } system_status = IRRIGATION_STATUS_INIT;



/*! Set color of indicator LED
@todo Move this to the events API */
void status_color(int r, int g, int b) {
	ws2812_set_led(&StatusLED, 0, LED_Gamma[r], LED_Gamma[g], LED_Gamma[b]);
	ws2812_update_blocking(&StatusLED);
}


// void status_led_disp_init(float dt) {
// 	static float timer = 0.0;
// 	static int m = 0;
// 	timer += dt;

// 	if (m==0) {

// 		float s = MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(150*s, 100*s, 0);
// 		if (timer > 0.1) {
// 			m = 1;
// 			timer=0;
// 		}
// 	} else {
// 		float s = 1.0 - MAX(MIN(timer*2.0, 1.0), 0.0);
// 		status_color(150*s, 100*s, 0);
// 		if (timer > 0.5) {
// 			m = 0;
// 			timer=0;
// 		}
// 	}


// }


// void status_led_disp_sensor_valid(float dt) {
// 	static float timer = 0.0;
// 	static int m = 0;
// 	timer += dt;

// 	if (m==0) {

// 		float s = MAX(MIN(timer*5.0, 1.0), 0.0);
// 		status_color(100*s, 150*s, 0);
// 		if (timer > 0.2) {
// 			m = 1;
// 			timer=0;
// 		}
// 	} else {
// 		float s = 1.0 - MAX(MIN(timer*5.0, 1.0), 0.0);
// 		status_color(100*s, 150*s, 0);
// 		if (timer > 0.2) {
// 			m = 0;
// 			timer=0;
// 		}
// 	}


// }

// void status_led_disp_sensor_stabilized(float dt) {
// 	static float timer = 0.0;
// 	static int m = 0;
// 	timer += dt;

// 	if (m==0) {

// 		float s = MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(100*s, 255*s, 100*s);
// 		if (timer > 0.1) {
// 			m = 1;
// 			timer=0;
// 		}
// 	} else if (m==1) {
// 		float s = 1.0 - MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(100*s, 255*s, 100*s);
// 		if (timer > 0.1) {
// 			m = 2;
// 			timer=0;
// 		}
// 	} else {
// 		status_color(0,0,0);
// 		if (timer > 5.0) {
// 			m = 0;
// 			timer=0;
// 		}

// 	}

// }

// void status_led_disp_fault_sensor(float dt) {
// 	static float timer = 0.0;
// 	static int m = 0;
// 	timer += dt;

// 	if (m==0) {

// 		float s = MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(255*s, 0, 0);
// 		if (timer > 0.1) {
// 			m = 1;
// 			timer=0;
// 		}
// 	} else {
// 		float s = 1.0 - MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(255*s, 0, 0);
// 		if (timer > 0.1) {
// 			m = 0;
// 			timer=0;
// 		}
// 	}

// }


// void status_led_disp_sensor_measuring(float dt) {
// 	static float timer = 0.0;
// 	static int m = 0;
// 	timer += dt;

// 	if (m==0) {

// 		float s = MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(50*s, 0, 100*s);
// 		if (timer > 0.1) {
// 			m = 1;
// 			timer=0;
// 		}
// 	} else {
// 		float s = 1.0 - MAX(MIN(timer*10.0, 1.0), 0.0);
// 		status_color(50*s, 0, 100*s);
// 		if (timer > 0.1) {
// 			m = 0;
// 			timer=0;
// 		}
// 	}

// }


/*! Send broken down time using blocking/polling (no dma/irq) */
void usart_blocking_tm(struct usart* usart, struct sw_timer_system_time* tm) {
	struct tm res;
	time_tm_from_epoch(&res, tm->epoch);	
	usart_blocking_int(usart, res.tm_year+1900);
	usart_blocking_str(usart, "-");
	usart_blocking_int_zp(usart, res.tm_mon + 1, 2);
	usart_blocking_str(usart, "-");
	usart_blocking_int_zp(usart, res.tm_mday, 2);
	usart_blocking_str(usart, " ");

	usart_blocking_int_zp(usart, res.tm_hour, 2);
	usart_blocking_str(usart, ":");
	usart_blocking_int_zp(usart, res.tm_min, 2);
	usart_blocking_str(usart, ":");
	usart_blocking_int_zp(usart, res.tm_sec, 2);
	usart_blocking_str(usart, ".");
	usart_blocking_int_zp(usart, tm->ms, 3);
}


struct state {
	void (*enter)(struct irrigation_controller*);
	void (*run)(struct irrigation_controller*);
	void (*exit)(struct irrigation_controller*);
	char* name;
};


const struct state state_init = {state_init_enter, state_init_run, state_init_exit, "Init"};
const struct state state_validate = {state_validate_enter, state_validate_run, state_validate_exit, "Validate temperature sensor"};
const struct state state_reboot = {state_reboot_enter, state_reboot_run, state_reboot_exit, "Reboot"};
const struct state state_measure = {state_measure_enter, state_measure_run, state_measure_exit, "Measure soil water content"};


struct state* get_state(enum irrigation_status status) {
	switch (status) {
		case irrigation_status_INIT:		
			return &state_init;
		case irrigation_status_VALIDATE:	
			return &state_validate;
		case irrigation_status_REBOOT:	
			return &state_reboot;
		case irrigation_status_MEASURE:	
			return &state_measure;
	}
	return 0;
}


/*! @copydoc irrigation_events::report_current_temperature */
void report_current_temperature(struct irrigation_controller* controller) {
	usart_blocking_tm(&Serial, &controller->status.iteration_time);
	usart_blocking_str(&Serial, ">  Current Sensor Temperature: ");
	usart_blocking_float(&Serial, controller->status.soil_temperature.temperature);
	usart_blocking_str(&Serial, "°C\n");
}

/*! @copydoc irrigation_events::report_validation_temperatures */
void report_validation_temperatures(struct irrigation_controller* controller) {
	usart_blocking_tm(&Serial, &controller->status.iteration_time);
	usart_blocking_str(&Serial, ">  Temperature Validation Report:\n\tMinimum:  ");
	usart_blocking_float(&Serial, controller->status.min_temperature);
	usart_blocking_str(&Serial, "°C\tMaximum:  ");
	usart_blocking_float(&Serial, controller->status.max_temperature);
	usart_blocking_str(&Serial, "°C\tΔT: ");
	usart_blocking_float(&Serial, controller->status.max_temperature - controller->status.min_temperature);
	usart_blocking_str(&Serial, "°C\n");
}

/*! @copydoc irrigation_events::report_sensor_malfunction */
void report_sensor_malfunction(struct irrigation_controller* controller) {
	usart_blocking_tm(&Serial, &controller->status.iteration_time);
	usart_blocking_str(&Serial, ">  ERROR! Temperature sensor offline\n");
}

/*! @copydoc irrigation_events::report_sensor_fluctuations */
void report_sensor_fluctuations(struct irrigation_controller* controller) {
	usart_blocking_tm(&Serial, &controller->status.iteration_time);
	usart_blocking_str(&Serial, ">  Warning! Temperature sensor is fluctuating\n");
}

/*! @copydoc irrigation_events::report_msg_note */
void report_msg_note(struct irrigation_controller* controller, char* msg) {
	usart_blocking_tm(&Serial, &controller->status.iteration_time);
	usart_blocking_str(&Serial, ">  Note: ");
	usart_blocking_strz(&Serial, msg);
	usart_blocking_str(&Serial, "\n");
}

/*! @copydoc irrigation_events::report_measurement_data */
void report_measurement_data(struct irrigation_controller* controller, char* msg) {
	usart_blocking_tm(&Serial, &controller->status.iteration_time);
	usart_blocking_str(&Serial, ">  Measurement complete.\n\t∑(|ΔTemperature|) = ");
	usart_blocking_float(&Serial, controller->status.ackumulative_delta_temp);
	usart_blocking_str(&Serial, "°C\tTime: ∑(ΔTime) = ");
	usart_blocking_float(&Serial, controller->status.ackumulative_delta_time);
	usart_blocking_str(&Serial, "°C\tChange: ∑(|ΔTemperature|)/∑(ΔTime) = ");
	usart_blocking_float(&Serial, controller->status.ackumulative_delta_temp / controller->status.ackumulative_delta_time);
	usart_blocking_str(&Serial, "°C/S\n");
}

/*! This is the main irrigation controller state */
struct irrigation_controller ic = {
	.events = {
		.report_validation_temperatures = report_validation_temperatures,
		.report_sensor_malfunction = report_sensor_malfunction,
		.report_sensor_fluctuations = report_sensor_fluctuations,
		.report_current_temperature = report_current_temperature,
		.report_msg_note = report_msg_note,
		.report_measurement_data = report_measurement_data,
	}
};


int main(void) {

	//Setup main clock
	rcc_clock_setup_in_hsi_out_24mhz();
	//Enable perhipherals
	rcc_periph_clock_enable(RCC_AFIO);

	//Initiate all hardware
	hw_init();	


	get_system_time_blocking(&ic.status.iteration_time);

	usart_blocking_tm(&Serial, &ic.status.iteration_time);
	usart_blocking_str(&Serial, ">  System starting\n");

	
	

	// float NTC_filter_error_timer;
	// float System_wait_sensor_stabilization_timer;

	// float Heater_timer=0;
	// float Heater_hot_time=0;
	// float soil_temperature;
	// float sensor_hot_temperature;
	// float sensor_cooldown_timer;

	timer_ccr_set(&Pump, 0);
	timer_ccr_set(&Heater, 0);

	get_system_time_blocking(&ic.status.iteration_time);

	ic.current_state = irrigation_status_UNDEFINED;
	ic.pending_state = irrigation_status_INIT;


	while(1) {

		/** @defgroup state_machine The main state machine */
		/** @{ */
		ic.status.previous_iteration_time = ic.status.iteration_time;
		get_system_time_blocking(&ic.status.iteration_time);
		ic.status.dt = (float)(ic.status.iteration_time.epoch - ic.status.previous_iteration_time.epoch) + (float)(ic.status.iteration_time.ms - ic.status.previous_iteration_time.ms) / 1000.0;



		if (ic.pending_state != irrigation_status_UNDEFINED) {
			struct state* state = get_state(ic.current_state);
			if (state) {

				usart_blocking_tm(&Serial, &ic.status.iteration_time);
				usart_blocking_str(&Serial, ">  Exiting state ");
				usart_blocking_strz(&Serial, state->name);
				usart_blocking_str(&Serial, "\n");
				state->exit(&ic);
			}

			ic.current_state = ic.pending_state;
			ic.pending_state = irrigation_status_UNDEFINED;

			state = get_state(ic.current_state);
			if (state) {
				usart_blocking_tm(&Serial, &ic.status.iteration_time);
				usart_blocking_str(&Serial, ">  Entering state ");
				usart_blocking_strz(&Serial, state->name);
				usart_blocking_str(&Serial, "\n");
				state->enter(&ic);
			}

		}
		/** }@ */



		/** @defgroup hardware_control Where the hardware is controlled */
		/** @{ */


		//Heater can only be controlled when state is in MEASURE
		if (ic.current_state == irrigation_status_MEASURE) {
			timer_ccr_set(&Heater, ic.heater);
		} else {
			timer_ccr_set(&Heater, 0);
		}


		//Perform measurements
		if (NTC_filtered_value != -1) {
			//We have valid temperature samples

			float s = sqrt((float)NTC_filtered_value);
			float ntc_resistance = R1 * s / (4095.0 - s);
			float v = ntc_resistance / NTC_R;
			ic.status.soil_temperature.temperature = 1.0 / (NTC_A1 + NTC_B1 * log(v) + NTC_C1 * log2(v) + NTC_D1 * cbrt(v)) - T0_K;
			ic.status.soil_temperature.timestamp = ic.status.iteration_time;


		} 



		/** }@ */



		/** @ingroup state_machine */
		/** @{ */

		struct state* state = get_state(ic.current_state);
		if (state) {
			state->run(&ic);
		} else {
			usart_blocking_tm(&Serial, &ic.status.iteration_time);
			usart_blocking_str(&Serial, ">  FATAL - Undefined State - System reboot!\n");
			ic.pending_state = irrigation_status_REBOOT;
		}



		/** }@ */



		// switch (system_status) {
		// 	case IRRIGATION_STATUS_INIT:
		// 		status_led_disp_init(time_diff);
		// 		break;
		// 	case IRRIGATION_STATUS_SENSOR_VALID:
		// 		status_led_disp_sensor_valid(time_diff);
		// 		break;

		// 	case IRRIGATION_STATUS_MEASURE_COMPLETE:
		// 	case IRRIGATION_STATUS_SENSOR_STABILIZED:
		// 		status_led_disp_sensor_stabilized(time_diff);
		// 		break;

		// 	case IRRIGATION_FAULT_SENSOR:
		// 		status_led_disp_fault_sensor(time_diff);
		// 		break;

		// 	case IRRIGATION_STATUS_MEASURE_START:
		// 	case IRRIGATION_STATUS_MEASURE_HOT:
		// 		status_led_disp_sensor_measuring(time_diff);
		// 		break;

		// }


		// if (system_status != IRRIGATION_STATUS_MEASURE_START) {
		// 	//Heater can only be on during measurements
		// 	timer_ccr_set(&Heater, 0);			
		// }


		// if (NTC_filtered_value != -1) {
		// 	//We have valid temperature samples

		// 	float s = sqrt((float)NTC_filtered_value);
		// 	float ntc_resistance = R1 * s / (4095.0 - s);
		// 	float v = ntc_resistance / NTC_R;
		// 	float t = 1.0 / (NTC_A1 + NTC_B1 * log(v) + NTC_C1 * log2(v) + NTC_D1 * cbrt(v)) - T0_K;

		// 	NTC_filter_error_timer = 0.0;
		// 	if (system_status < IRRIGATION_STATUS_SENSOR_VALID) {	//Promote system status if needed
		// 		system_status = IRRIGATION_STATUS_SENSOR_VALID;
		// 		System_wait_sensor_stabilization_timer = 0.0;
		// 		usart_blocking_tm(&Serial, &previous_iteration_time);
		// 		usart_blocking_str(&Serial, ">  System status changed to sensor valid\n");
		// 	} else if (system_status == IRRIGATION_STATUS_SENSOR_VALID) {
		// 		System_wait_sensor_stabilization_timer += time_diff;
		// 		if (System_wait_sensor_stabilization_timer > 3.0) {	//3.0 for testing
		// 			usart_blocking_tm(&Serial, &previous_iteration_time);
		// 			usart_blocking_str(&Serial, ">  System status changed to sensor stabilized\n");
		// 			system_status = IRRIGATION_STATUS_SENSOR_STABILIZED;
		// 		}

		// 	} else if (system_status == IRRIGATION_STATUS_SENSOR_STABILIZED) {


		// 		soil_temperature = t;
		// 		usart_blocking_tm(&Serial, &previous_iteration_time);
		// 		usart_blocking_str(&Serial, ">  Starting measurement, soil temperature is ");
		// 		usart_blocking_float(&Serial, soil_temperature);
		// 		usart_blocking_str(&Serial, "°C\n");
		// 		system_status = IRRIGATION_STATUS_MEASURE_START;
		// 		Heater_timer = 0;
		// 		Heater_hot_time=0.0;

		// 	} else if (system_status == IRRIGATION_STATUS_MEASURE_START) {


		// 		if (Heater_timer < 1.0) {
		// 			timer_ccr_set(&Heater, Heater_timer*4095.0);
		// 			Heater_timer+=time_diff;

		// 		} else {
		// 			timer_ccr_set(&Heater, 4095);

		// 			if ((Heater_hot_time > 20.0) || (t > soil_temperature + 2.0)) {	//Try to heat 3 degrees
		// 				//Turn off heater

		// 				// float dps = (t - soil_temperature) / Heater_hot_time;
		// 				// float water_content = 0.00242 / dps;

		// 				sensor_hot_temperature = t;
		// 				sensor_cooldown_timer=0;
		// 				timer_ccr_set(&Heater, 0);
		// 				usart_blocking_tm(&Serial, &previous_iteration_time);
		// 				usart_blocking_str(&Serial, ">  Sensor hot, temperature is ");
		// 				usart_blocking_float(&Serial, t);
		// 				usart_blocking_str(&Serial, "°C,\t");
		// 				usart_blocking_str(&Serial, "Heating time: ");
		// 				usart_blocking_float(&Serial, Heater_hot_time);					
		// 				usart_blocking_str(&Serial, "S\n");

		// 				// usart_blocking_str(&Serial, "S,\tDPS: ");						
		// 				// usart_blocking_float(&Serial, dps);
		// 				// usart_blocking_str(&Serial, "°C/S,\tWater content: ");
		// 				// usart_blocking_float(&Serial, water_content*100.0);
		// 				// usart_blocking_str(&Serial, "%\n");
		// 				system_status = IRRIGATION_STATUS_MEASURE_HOT;
		// 			}
				
		// 			Heater_hot_time+=time_diff;
		// 		}


		// 	} else if (system_status == IRRIGATION_STATUS_MEASURE_HOT) {
				
		// 		sensor_cooldown_timer += time_diff;
		// 		if ((t < sensor_hot_temperature - 1.0) || sensor_cooldown_timer > 30.0) {		//1 degree cooldown


		// 			if (sensor_hot_temperature - t < 0.1) {

		// 				usart_blocking_tm(&Serial, &previous_iteration_time);
		// 				usart_blocking_str(&Serial, ">  Moisture reading failed");
		// 				system_status = IRRIGATION_STATUS_MEASURE_COMPLETE;	//TODO: försöka igen

		// 			} else {

		// 				float mdps = (sensor_hot_temperature - t) *1000.0 / sensor_cooldown_timer;
		// 				float water_content = 242.0/mdps;

		// 				usart_blocking_tm(&Serial, &previous_iteration_time);
		// 				usart_blocking_str(&Serial, ">  Sensor cool, temperature is ");
		// 				usart_blocking_float(&Serial, t);
		// 				usart_blocking_str(&Serial, "°C,\t");
		// 				usart_blocking_str(&Serial, "Cooldown time: ");
		// 				usart_blocking_float(&Serial, sensor_cooldown_timer);					
		// 				usart_blocking_str(&Serial, "S,\tΔT: ");
		// 				usart_blocking_float(&Serial, sensor_hot_temperature - t);
		// 				usart_blocking_str(&Serial, "°C,\tMDPS: ");
		// 				usart_blocking_float(&Serial, mdps);					
		// 				usart_blocking_str(&Serial, "°mC/s,\tWater content: ");
		// 				usart_blocking_float(&Serial, water_content*100.0);					
		// 				usart_blocking_str(&Serial, "%\n");


		// 				system_status = IRRIGATION_STATUS_MEASURE_COMPLETE;
		// 			}
		// 		}

		// 	}


		// } else {
		// 	//No valid temperature samples
		// 	NTC_filter_error_timer += time_diff;
			
		// 	if (NTC_filter_error_timer > 0.5) {
		// 		if (system_status != IRRIGATION_FAULT_SENSOR) {
		// 			system_status = IRRIGATION_FAULT_SENSOR;
		// 			usart_blocking_tm(&Serial, &previous_iteration_time);
		// 			usart_blocking_str(&Serial, ">  System status changed to broken sensor\n");
		// 		}
				
		// 		NTC_filter_error_timer = 0;
				

		// 	}

		// }
		

	}

	return 0;
}





			// float s = sqrt((float)NTC_filtered_value);

			// float ntc_resistance = R1 * s / (4095.0 - s);

			// float v = ntc_resistance / NTC_R;
			// float t = 1.0 / (NTC_A1 + NTC_B1 * log(v) + NTC_C1 * log2(v) + NTC_D1 * cbrt(v)) - T0_K;


			// usart_blocking_str(&Serial, "Temperature: ");
			// usart_blocking_float(&Serial, t);
			// usart_blocking_str(&Serial, "°C\t\t");

			// usart_blocking_str(&Serial, "Resistance: ");
			// usart_blocking_float(&Serial, ntc_resistance);
			// usart_blocking_str(&Serial, "Ω\t\t");

			// usart_blocking_str(&Serial, "Raw: ");
			// usart_blocking_int(&Serial, (int)s);
			// usart_blocking_str(&Serial, "\n");

