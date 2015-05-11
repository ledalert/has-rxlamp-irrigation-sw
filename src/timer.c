#include "timer.h"


void timer_ccr_init(struct timer_ccr* ccr, enum hw_init_state state) {

	timer_init(ccr->configuration->timer, state);
	ccr->state = state;

	uint32_t CCER_SET = 0;
	uint32_t CCER_CLEAR = 0;
	uint32_t CCMR2_SET = 0;

	switch (state) {
		case HW_INIT_RCC:
			break;

		case HW_INIT_PRE_NVIC:
			/* Polarity and state */

			switch (ccr->configuration->channel) {
				case TIM_OC3N:
					CCER_SET = TIM_CCER_CC3NE;
					CCER_CLEAR = TIM_CCER_CC3NP;
					CCMR2_SET = TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;
					TIM_CCR3(ccr->configuration->timer->configuration->timer) = ccr->configuration->initial_reload;


			}
			TIM_CCER(ccr->configuration->timer->configuration->timer) |= CCER_SET;
			TIM_CCER(ccr->configuration->timer->configuration->timer) &= ~CCER_CLEAR;
			TIM_CCMR2(ccr->configuration->timer->configuration->timer) |= CCMR2_SET;


			break;



		default:
			break;
	}

}

void timer_init(struct timer* timer, enum hw_init_state state) {
	timer->state = state;
	switch (state) {
		case HW_INIT_RCC:
			rcc_periph_clock_enable(timer->configuration->rcc);	
			break;

		case HW_INIT_PRE_NVIC:

			TIM_CR1(timer->configuration->timer) = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
			/* Period */
			TIM_ARR(timer->configuration->timer) = timer->configuration->initial_reload;
			timer->reload = timer->configuration->initial_reload;
			/* Prescaler */
			TIM_PSC(timer->configuration->timer) = 0;
			TIM_EGR(timer->configuration->timer) = TIM_EGR_UG;

			if (timer->configuration->timer == TIM1) {
				TIM_BDTR(timer->configuration->timer) |= TIM_BDTR_MOE; //Advanced timer have master output enable
			}
			
			/* ARR reload enable */
			TIM_CR1(timer->configuration->timer) |= TIM_CR1_ARPE;

			break;

		case HW_INIT_POST_INIT:

			/* Counter enable */
			TIM_CR1(timer->configuration->timer) |= TIM_CR1_CEN;
			break;


		default:
			break;
	}
}