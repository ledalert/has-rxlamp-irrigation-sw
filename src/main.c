/*! \file main.c
    \brief Irrigation controller main file
 
 	@todo	systick, adc, moisture sensor, pwm output, filter
 	@todo   explain port and rcc better

	@mainpage Irrigation Controller
	@section index_sec Index

	@li main.c Main file


*/


#define HW_PRE_INIT(call, obj)\
	call(obj, HW_INIT_RCC);\
	call(obj, HW_INIT_GPIO);\
	call(obj, HW_INIT_PRE_NVIC);\
	call(obj, HW_INIT_NVIC);\
	call(obj, HW_INIT_POST_INIT);


#include "math.h"
#include "ws2812_macro.h"
#include "usart_macro.h"
#include "dma_macro.h"

#include "led_macro.h"
#include "timer_macro.h"
#include "systick_macro.h"
#include "adc.h"
#include "time.h"
#include "filter.h"
#include "irrigation.h"

//Create hardware configuration
GPIO_PORT_INSTANCE(GPIO_PA, GPIOA, RCC_GPIOA);
GPIO_PORT_INSTANCE(GPIO_PB, GPIOB, RCC_GPIOB);

GPIO_PIN_NOCONF(Serial_tx, &GPIO_PA, GPIO_USART1_TX);
USART_8N1(Serial, USART1, RCC_USART1, 921600, 0, &Serial_tx);

DMA_INSTANCE(Dma, DMA1, RCC_DMA1);

GPIO_PIN_NOCONF(StatusLED_pin, &GPIO_PB, GPIO15);
TIMER_NOCONF(StatusLED_timer, TIM1, RCC_TIM1);
DMA_CHANNEL_NOCONF(StatusLED_dma, &Dma, DMA_CHANNEL6);
TIMER_CCR_NOCONF(StatusLED_pwm, &StatusLED_timer, TIM_OC3N, &StatusLED_dma);

WS2812_SIMPLE_CONF(StatusLED, &StatusLED_pin, &StatusLED_pwm, 1);


SYSTICK_AUTO_CONFIG(Systick, 1000);

GAMMA_LUT_8_8_E_INSTANCE(LED_Gamma);


volatile int sleep_ms = 0;	/*!< Holds number of ms to sleep, counts down in sys_tick_handler() */
volatile int NTC_filtered_value = -1;


/*! Init hardware */
void hw_init_state(enum hw_init_state state) {
	//Init hardware
	ws2812_init(&StatusLED, state);
	usart_init(&Serial, state);
	systick_init(&Systick, state);
}



volatile struct sw_timer_system_time system_time = {1420070400, 0};
volatile bool system_time_updated=0;

/*! Systick IRQ handler */
void sys_tick_handler(void) {
	if (sleep_ms) {
		sleep_ms--;
	}

	if (++system_time.ms == 1000) {	
		system_time.epoch++;
		system_time.ms=0;
	}
	system_time_updated = 1;

}

/*! Sleep in millisecond durations */
void ms_sleep(int time) {
	sleep_ms = time;
	while (sleep_ms);
}

/*! Get time, wait until an atomic reading was successful */
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


/*! Update status LED */
void set_status_color(int r, int g, int b) {
	ws2812_set_led(&StatusLED, 0, LED_Gamma[r], LED_Gamma[g], LED_Gamma[b]);
	ws2812_update_blocking(&StatusLED);
}


/*! Application entry point */
int main(void) {

	//Setup main clock
	rcc_clock_setup_in_hsi_out_24mhz();
	//Enable perhipherals
	rcc_periph_clock_enable(RCC_AFIO);


	//Initiate all hardware
	hw_init();

	//Say hello
	struct sw_timer_system_time now;
	get_system_time_blocking(&now);
	usart_blocking_tm(&Serial, &now);
	usart_blocking_str(&Serial, ">  System starting\n");

	while (1) {
		set_status_color(255, 0, 0);	
		ms_sleep(250);
		set_status_color(0, 255, 0);	
		ms_sleep(250);
		set_status_color(0, 0, 255);	
		ms_sleep(250);

	}

	
	return 0;
}
