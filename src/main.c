
/*
PB11 timer 2 kanal 4 vid remap
Nät: POWERDATA

AFIO_MAPR_SWJ_CFG_FULL_SWJ, AFIO_MAPR_TIM2_REMAP_PARTIAL_REMAP2 

*/

#define MSG_MEASURED_TIME 1
#define MSG_MEASURED_TEMP 2
#define MSG_TM_STRUCT 3

#define T_MEAS_LOW		30.0
#define T_MEAS_HIGH		30.25

#define VREF 3.295
#define R1 100000.0

#define NTC_R	150000
#define NTC_A1	3.354016E-03
#define NTC_B1	2.367720E-04
#define NTC_C1	3.585140E-06
#define NTC_D1	1.255349E-07
#define T0_K	273.15


#define BIT_0 6
#define BIT_1 14
#define FREQ 800000

#define NUMBER_LEDS 1

#define PWM_PERIOD 4096
#define PWM_TOP (PWM_PERIOD - 1)



#define MAX(v, x) ((v > x) ? v : x)
#define MIN(v, x) ((v < x) ? v : x)


// #define BIT_0 8
// #define BIT_1 29
// #define FREQ 400000

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <math.h>
#include <time.h>

volatile int adc_ready;
volatile int ms_time_delay;

struct color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

volatile uint8_t led_output_buf[NUMBER_LEDS*24+1];	//42 x 24
volatile struct color led_input_buf[NUMBER_LEDS];


volatile uint16_t* adc_samples[1];
volatile int current_temperature=-1;

struct sample_range {
	int min;
	int max;
};

volatile int cooloff_timer = -1;
int __secs_to_tm(int t, struct tm *tm);

struct sample_buffer {
	volatile int* buffer;
	int size;								//Size of the buffer
	int pos;								//Current position of the buffer
	int count;								//Count of samples in buffer, used to determine if data is filtered yet
	int sqsum;								//Total square sum of samples
	int invalid_samples_streak;				//Used for detecting sensor malfunction
	struct sample_range valid_range;
};



volatile int system_time = 1420070400;


volatile int mscounter=0;



void send_float(float value);
void send_int(int value);

int buffer_add_sample(volatile struct sample_buffer* data, int sample) {
	int result = -1;
	if ((sample >= data->valid_range.min) && (sample <= data->valid_range.max)) {

		if (data->count < data->size) {
			data->count++;

		} else {
			data->sqsum -= data->buffer[data->pos];	//Remove tail from result (pos is both head and tail)
		}

		data->buffer[data->pos] = sample * sample;
		data->sqsum += data->buffer[data->pos];	//Add square of current sample to sum

		if (data->count == data->size) {
			result = data->sqsum;			//Return square sum
		}

		data->invalid_samples_streak = 0;
		data->pos = (data->pos + 1) % data->size; //advance position

	} else {
		data->invalid_samples_streak++;
	}
	return result;
}






void dma1_transmit(uint32_t src, uint32_t dst, uint32_t length, uint32_t channel) {
	dma_channel_reset(DMA1, channel);

	dma_set_peripheral_address(DMA1, channel, dst);
	dma_set_memory_address(DMA1, channel, src);
	dma_set_number_of_data(DMA1, channel, length);
	dma_set_read_from_memory(DMA1, channel);
	dma_enable_memory_increment_mode(DMA1, channel);
	dma_set_peripheral_size(DMA1, channel, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, channel, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, channel, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, channel);

}


void dma1_transmit_8_32(uint32_t src, uint32_t dst, uint32_t length, uint32_t channel) {

	dma_disable_channel(DMA1, channel);

	dma_channel_reset(DMA1, channel);

	dma_set_peripheral_address(DMA1, channel, dst);
	dma_set_memory_address(DMA1, channel, src);
	dma_set_number_of_data(DMA1, channel, length);
	dma_set_read_from_memory(DMA1, channel);
	dma_enable_memory_increment_mode(DMA1, channel);
	dma_set_peripheral_size(DMA1, channel, DMA_CCR_PSIZE_32BIT);
	dma_set_memory_size(DMA1, channel, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, channel, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, channel);

}


void dma1_recieve(uint32_t src, uint32_t dst, uint32_t length, uint32_t channel) {
	
	dma_disable_channel(DMA1, channel);
	dma_channel_reset(DMA1, channel);

	dma_set_peripheral_address(DMA1, channel, src);
	dma_set_memory_address(DMA1, channel, dst);
	dma_set_number_of_data(DMA1, channel, length);
	dma_set_read_from_peripheral(DMA1, channel);
	dma_enable_memory_increment_mode(DMA1, channel);
	dma_set_peripheral_size(DMA1, channel, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, channel, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, channel, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, channel);

}





void adc_sample() {

	if (adc_ready) {

		dma_channel_reset(DMA1, DMA_CHANNEL1);

		dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t)&ADC1_DR);
		dma_set_memory_address(DMA1, DMA_CHANNEL1,(uint32_t)adc_samples);
		dma_set_number_of_data(DMA1, DMA_CHANNEL1, 1);
		dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
		dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
		dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
		dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
		dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
		dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_VERY_HIGH);

		dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);

		dma_enable_channel(DMA1, DMA_CHANNEL1);

		adc_start_conversion_regular(ADC1);
	}
}



void sys_tick_handler(void) {
	if (ms_time_delay) {
		ms_time_delay--;
	}

	if (cooloff_timer != -1) {
		cooloff_timer++;
	}
	
	if (++mscounter == 1000) {
		mscounter = 0;
		system_time++;
		
	}

	adc_sample();

}

void sleep_ms(int t) {
	ms_time_delay = t;	while (ms_time_delay);
}

inline void colorHexagon(int hue, int *R, int *G, int *B) {
	int frac = hue >> 12;
	int ci = hue & 0xFFF;
	int cd = 4095 - ci;
	int cs = 4095;
	switch (frac) {
		case 0:	*R = cs;	*G = ci;	*B = 0; break;		//R1	G+	B0
		case 1:	*R = cd;	*G = cs;	*B = 0; break;		//R-	G1	B0
		case 2:	*R = 0;	*G = cs;	*B = ci; break;	//R0	G1	B+
		case 3:	*R = 0;	*G = cd;	*B = cs; break;	//R0	G-	B1
		case 4:	*R = ci;	*G = 0;	*B = cs; break;	//R+	G0	B1
		case 5:	*R = cs;	*G = 0;	*B = cd; break;	//R1	G0	B-
	}
}

static unsigned long xorshf96(void) {    /* A George Marsaglia generator, period 2^96-1 */
	static unsigned long x=123456789, y=362436069, z=521288629;
	unsigned long t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;

	z = t ^ x ^ y;
	return z;
}

void init_rcc() {
	rcc_clock_setup_in_hsi_out_24mhz();
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_ADC1);


}





void init_gpio() {

	//gpio_primary_remap(AFIO_MAPR_SWJ_CFG_FULL_SWJ, AFIO_MAPR_TIM2_REMAP_PARTIAL_REMAP2);


	//GP1 - resistor towards 3v3 and ntc towards gnd
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO3);



	//ws2812b
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO15);

	//Pump
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);

	//Heater
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);


	//usart
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

}

void init_adc() {
	adc_off(ADC1);

	adc_enable_scan_mode(ADC1);	
	adc_set_single_conversion_mode(ADC1);

	adc_set_right_aligned(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC);
	adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
	adc_enable_dma(ADC1);
	
	adc_power_on(ADC1);


	sleep_ms(2);	//Sleeping 1 ms may be less than one ms if systick is about to happen, 
	// we actually just need to wait 3µS according to http://libopencm3.github.io/docs/latest/stm32f1/html/group__adc__file.html#ga51f01f6dedbcfc4231e0fc1d8943d956

	adc_reset_calibration(ADC1);
	adc_calibration(ADC1);

	uint8_t channel_array[1] = {3};
	
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_ready = 1;
}

void init_systick() {
	ms_time_delay=0;
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(24000-1);  // 1 kHz
	systick_interrupt_enable();
	systick_counter_enable();


}

void init_timer() {

	//THis is for the ws2812b

	TIM1_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	/* Period */
	TIM1_ARR = (24000000 / FREQ)-1;
	/* Prescaler */
	TIM1_PSC = 0;
	TIM1_EGR = TIM_EGR_UG;

	TIM1_BDTR |= TIM_BDTR_MOE; //test


	/* Polarity and state */
	TIM1_CCER |= TIM_CCER_CC3NE;
	TIM1_CCER &= ~TIM_CCER_CC3NP;
	TIM1_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;


	/* ARR reload enable */
	TIM1_CR1 |= TIM_CR1_ARPE;


	/* Counter enable */
	TIM1_CR1 |= TIM_CR1_CEN;

	//TIM1_CCR3 = 3;






	//This is for the pump
//Configuring timer TIM3
	TIM3_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM3_ARR = PWM_TOP;
	TIM3_PSC = 0;
	TIM3_EGR = TIM_EGR_UG;
	//Setup output compare
		TIM3_CCR3 = 0;	//Pump starts off
		TIM3_CCER |=  TIM_CCER_CC3E ;
		TIM3_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE ;

	TIM3_CR1 |= TIM_CR1_ARPE;
	TIM3_CR1 |= TIM_CR1_CEN;

	//This is for the heater
//Configuring timer TIM2
	TIM2_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM2_ARR = PWM_TOP;
	TIM2_PSC = 0;
	TIM2_EGR = TIM_EGR_UG;
	//Setup output compare
		TIM2_CCR3 = 0;	//heater starts off
		TIM2_CCER |=  TIM_CCER_CC3E ;
		TIM2_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE ;

	TIM2_CR1 |= TIM_CR1_ARPE;
	TIM2_CR1 |= TIM_CR1_CEN;




}

void init_usart() {
	usart_set_baudrate(USART1, 921600);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_RX | USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);


}

void init_nvic() {

	nvic_set_priority(NVIC_DMA1_CHANNEL1_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

}

void init_stuff() {
	init_rcc();
	init_gpio();
	init_systick();
	init_timer();
	init_usart();
	init_adc();
	init_nvic();
}


void populate_test_buffer() {
	// start populate buffer
	for (int led=0;led<NUMBER_LEDS;led++) {
		led_input_buf[led].r = led * 1;
		led_input_buf[led].g = led * 2;
		led_input_buf[led].b = led * 3;
	}

}

void process_buffer() {
	volatile uint8_t* ptr=led_output_buf;
	for (int led=0;led<NUMBER_LEDS;led++) {

		for (int bit=7;bit>=0;bit--) { *ptr++ = led_input_buf[led].g & (1 << bit) ? BIT_1 : BIT_0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led_input_buf[led].r & (1 << bit) ? BIT_1 : BIT_0; }
		for (int bit=7;bit>=0;bit--) { *ptr++ = led_input_buf[led].b & (1 << bit) ? BIT_1 : BIT_0; }

	}

}



void send_led_buffer() {
	//Send led data

	
	//dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);

//	TIM2_DIER &= ~TIM_DIER_CC1DE;	//Disable DMA transfer for CC1
//	usart_disable_rx_dma(USART1);


	dma1_transmit_8_32((uint32_t) led_output_buf, (uint32_t) &TIM1_CCR3, 24*NUMBER_LEDS+1, DMA_CHANNEL6 );
	TIM1_DIER |= TIM_DIER_CC3DE;	//Enable DMA transfer for CC1

}

void uart_send_data(volatile void* data, int length) {

	uint8_t* ptr = (uint8_t*)data;
	for (int i=0; i<length; i++) {
		usart_send_blocking(USART1, ptr[i]);

	}

}


void send_led_buffer_blocking() {
	send_led_buffer();
	while (!dma_get_interrupt_flag(DMA1, DMA_CHANNEL6, DMA_TCIF));

	TIM1_DIER &= ~TIM_DIER_CC3DE;	//Disable DMA transfer for CC1

}


void recieve_led_data_blocking() {


	dma1_recieve((uint32_t)&USART1_DR, (uint32_t)led_input_buf, NUMBER_LEDS * sizeof(struct color), DMA_CHANNEL5);

	//dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
	usart_enable_rx_dma(USART1);

	while (!dma_get_interrupt_flag(DMA1, DMA_CHANNEL5, DMA_TCIF));

	usart_disable_rx_dma(USART1);


}


volatile int hue=0;


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


void set_indicator_rgb(int r, int g, int b) {
	led_input_buf[0].r = r;
	led_input_buf[0].g = g;
	led_input_buf[0].b = b;

	process_buffer();
	send_led_buffer_blocking();

}

#define LED_LIGHT_BLUE 50, 50, 100
#define LED_ORANGE 100, 50, 0
#define LED_GREEN 0, 100, 0




int main() {

	int heater_pwm = 0;


	init_stuff();
	
	set_indicator_rgb(LED_LIGHT_BLUE);


	while(1) {
		struct tm datetime;
		__secs_to_tm(system_time, &datetime);

		send_int(MSG_TM_STRUCT);
		send_int(datetime.tm_year);
		send_int(datetime.tm_mon);
		send_int(datetime.tm_mday);
		send_int(datetime.tm_hour);
		send_int(datetime.tm_min);
		send_int(datetime.tm_sec);
		send_int(datetime.tm_yday);
		send_int(datetime.tm_wday);

		sleep_ms(1000);

	}


	int temperature_time=0;

	int heater_mode = -1;
	float t_high = 0;

	while (1) {
		float t;
		int last_value = current_temperature;
		if (last_value != -1) {
			float adc_sample = sqrt((float)last_value / ntc_resistor.size) ;
			
			// float ntc_resistance = R1 * adc_sample * VREF / (4095.0 * VREF - (adc_sample * VREF));
			//x = (r * s * v) / (t * v - (s * v))

			float ntc_resistance = R1 * adc_sample / (4095.0 - adc_sample);



			float v = ntc_resistance / NTC_R;
			t = 1.0 / (NTC_A1 + NTC_B1 * log(v) + NTC_C1 * log2(v) + NTC_D1 * cbrt(v)) - T0_K;
			//(r * s) / (t - s)

			switch (heater_mode) {
				case -1:	//Begin with cooldown
					heater_mode = 0;
					set_indicator_rgb(LED_GREEN);				
					break;

				case 0:
					if (t <= T_MEAS_LOW) {						
						heater_mode = 1;
						heater_pwm = 0;

						if (cooloff_timer != -1) {
							float measured_time = (float)cooloff_timer/1000.0;
							send_int(MSG_MEASURED_TIME);
							send_float(t_high);
							send_float(t);
							send_float(measured_time);
						}

						cooloff_timer = -1; //stop timer
					}

					break;

				case 1:

					set_indicator_rgb(heater_pwm/50, heater_pwm/150, 0);


					if (heater_pwm < 4095) {
						heater_pwm = MIN(heater_pwm + 15, 4095);
						TIM2_CCR3 = heater_pwm;
					}


					if (t >= T_MEAS_HIGH) {
						t_high = t;
						heater_mode = 0;
						heater_pwm = 0;
						TIM2_CCR3 = heater_pwm;
						cooloff_timer = 0;		//Start timer

						set_indicator_rgb(LED_GREEN);
					}
					break;

			}



		}
		sleep_ms(10);


		if (++temperature_time > 1000) {		//Every 10 seconds
			temperature_time=0;
			send_int(MSG_MEASURED_TEMP);
			send_float(t);
		}
	}


	int pump_mode = 0;
	int pump_ramp = 0;


	while(1) {
	
	

		int r, g, b;

		colorHexagon(hue, &r, &g, &b);

		led_input_buf[0].r = r >> 4;
		led_input_buf[0].g = g >> 4;
		led_input_buf[0].b = b >> 4;

		process_buffer();
		send_led_buffer_blocking();


		switch (pump_mode) {
			case 0:
				TIM3_CCR3 = 0;
				if (++pump_ramp > 10000) {
					pump_mode = 1;
					pump_ramp = 0;
				}
				break;

			case 1:
				TIM3_CCR3 = pump_ramp;
				if (++pump_ramp > 4095) {
					pump_mode = 2;
					pump_ramp = 0;
				}
				break;

			case 2:
				TIM3_CCR3 = 4095;
				if (++pump_ramp > 3000) {
					pump_mode = 3;
					pump_ramp = 0;
				}
				break;

			case 3:
				TIM3_CCR3 = 4095-pump_ramp;
				if (++pump_ramp > 4095) {
					pump_mode = 0;
					pump_ramp = 0;
				}
				break;

		}


		sleep_ms(1);
	}

	return 0;
}



void dma1_channel1_isr(void) {
	DMA1_IFCR |= DMA_IFCR_CTCIF1;
	current_temperature = buffer_add_sample(&ntc_resistor, (int)adc_samples[0]);
}


void dma1_channel5_isr(void) {
	DMA1_IFCR |= DMA_IFCR_CTCIF5;


	// switch (dma_mode) {
	// 	case RECIEVE_LED_DATA:

	// 		TIM2_DIER &= ~TIM_DIER_CC1DE;	//Disable DMA transfer for CC1

	// 		dma_mode = SEND_LED_DATA;
	// 		dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);

	// 		dma1_transmit_8_32((uint32_t) buf, (uint32_t) &TIM2_CCR4, 24*NUMBER_LEDS+1, DMA_CHANNEL5 );
	// 		TIM2_DIER |= TIM_DIER_CC1DE;	//Enable DMA transfer for CC1
	// 		break;

	// 	case SEND_LED_DATA:

	// 		dma1_recieve((uint32_t)&USART1_DR, (uint32_t)led_data, 32, DMA_CHANNEL5);
	// 		dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
	// 		usart_enable_rx_dma(USART1);
	// 		break;
	// 	}
	
}



void send_float(float value) {
	volatile float data = value;
	uart_send_data(&data, sizeof(data));
}

void send_int(int value) {
	volatile int data = value;
	uart_send_data(&data, sizeof(data));
}



// http://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c

/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400*(31+29))

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)


int __secs_to_tm(int t, struct tm *tm) {
	long long days, secs;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int years, months;
	int wday, yday, leap;
	static const char days_in_month[] = {31,30,31,30,31,31,30,31,30,31,31,29};

	secs = t - LEAPOCH;
	days = secs / 86400;
	remsecs = secs % 86400;
	if (remsecs < 0) {
		remsecs += 86400;
		days--;
	}

	wday = (3+days)%7;
	if (wday < 0) wday += 7;

	qc_cycles = days / DAYS_PER_400Y;
	remdays = days % DAYS_PER_400Y;
	if (remdays < 0) {
		remdays += DAYS_PER_400Y;
		qc_cycles--;
	}

	c_cycles = remdays / DAYS_PER_100Y;
	if (c_cycles == 4) c_cycles--;
	remdays -= c_cycles * DAYS_PER_100Y;

	q_cycles = remdays / DAYS_PER_4Y;
	if (q_cycles == 25) q_cycles--;
	remdays -= q_cycles * DAYS_PER_4Y;

	remyears = remdays / 365;
	if (remyears == 4) remyears--;
	remdays -= remyears * 365;

	leap = !remyears && (q_cycles || !c_cycles);
	yday = remdays + 31 + 28 + leap;
	if (yday >= 365+leap) yday -= 365+leap;

	years = remyears + 4*q_cycles + 100*c_cycles + 400*qc_cycles;

	for (months=0; days_in_month[months] <= remdays; months++)
		remdays -= days_in_month[months];

	tm->tm_year = years + 100;
	tm->tm_mon = months + 2;
	if (tm->tm_mon >= 12) {
		tm->tm_mon -=12;
		tm->tm_year++;
	}
	tm->tm_mday = remdays + 1;
	tm->tm_wday = wday;
	tm->tm_yday = yday;

	tm->tm_hour = remsecs / 3600;
	tm->tm_min = remsecs / 60 % 60;
	tm->tm_sec = remsecs % 60;

	return 0;
}
