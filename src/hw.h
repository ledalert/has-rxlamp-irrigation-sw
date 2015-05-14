/*! \file hw.h
    \brief Hardware abstraction layer include file
    
    \todo There is some stuff not belonging to this file in here, it should be moved

    The hardware abstraction layer should be completely generic except that it initializes the system in a predefined order

*/

#ifndef _HW_H_

/*! Sets a value if the value evaluates to false
 	@param value Variable to check and possibly set 
 	@param default Default value if value was 0 */
#define DEFAULT(value, default) value = value ? value : default


#define VREF 3.295
#define R1 100000.0

#define NTC_R	150000
#define NTC_A1	3.354016E-03
#define NTC_B1	2.367720E-04
#define NTC_C1	3.585140E-06
#define NTC_D1	1.255349E-07
#define T0_K	273.15

#define PWM_PERIOD 4096
#define PWM_TOP (PWM_PERIOD - 1)

void hw_init();
/*! This is the state of the hardware abstraction layer */
enum hw_init_state {
	HW_INIT_UNINITIALIZED,
		/*!< Hardware is not initialized */
	HW_INIT_RCC,
		/*!< This is the first state entered and here
		hardware drivers may initialize any clocks needed */
	HW_INIT_GPIO,
		/*!< After clocks have been initialized
		all GPIO will be initialized, this also
		puts pins to their default value) */
		/*! @todo Implement the default value thing */

	HW_INIT_PRE_NVIC,
		/*!< After GPIOs have been initialized
		we might want to do some stuff before NVIC */
	HW_INIT_NVIC,
		/*!< After GPIO have been initialized
		all NVIC will be initialized */
	HW_INIT_POST_INIT, 
		/*!< After NVIC is initialized there might be more stuff
		todo such as setting up perhipheral settings */
	HW_INIT_INITIALIZED,
};


void hw_init_state(enum hw_init_state state); /*!< This function prototype calls all perhipherals with the state as argument */


#define _HW_H_
#endif