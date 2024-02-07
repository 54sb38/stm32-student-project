 /** 
 **************************************************************
 * @file mylib/s4595664_pantilt.c
 * @author Jingbo Ma - 45956649
 * @date 26032022
 * @brief Pantilt driver
 * REFERENCE: csse3010_mylib_reg_pantilt.pdf 
 *            stm32f429zi_reference.pdf
 *            stm32f429zi_datasheet.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_pantilt_init() - Init gpio and timer for PWM gen
 * s4595664_pantilt_angle_write() - set pantilt angle (-85 to 85 deg)
 * s4595664_pantilt_angle_read() - read current angle
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 * S4595664_REG_PANTILT_PAN_ZERO_CAL_OFFSET
 * S4595664_REG_PANTILT_TILT_ZERO_CAL_OFFSET
 ***************************************************************
 */

#include "s4595664_pantilt.h"

void pan_init(void);
void pan_write(int angle);
void tilt_init(void);
void tilt_write(int angle);

/** 
 * @brief   initialize gpio, timer
 * @param   void
 */
void s4595664_reg_pantilt_init(void) {

    pan_init();
	tilt_init();
}

/** 
 * @brief   set pantilt angle
 * @param   type: 0 = pan, 1 = tilt
 * @param 	angle: angle
 */
void s4595664_pantilt_angle_write(int type, int angle) {

    switch (type) {
        case 0:
            pan_write(angle);
            break;
		
		case 1:
			tilt_write(angle);
			break;
    }
}

/** 
 * @brief   read current angle
 * @param   type: 0 = pan, 1 = tilt
 */
int s4595664_pantilt_angle_read(int type) {

    int tick = 0;
    int offset = 0;
    switch (type) {
        case 0:
            tick = TIM1->CCR1;
            offset = S4595664_REG_PANTILT_PAN_ZERO_CAL_OFFSET;
            break;
		case 1:
            tick = TIM1->CCR2;
            offset = S4595664_REG_PANTILT_TILT_ZERO_CAL_OFFSET;
            break;
    }
    int angle = -180 * (tick - 145) / 200;
}

/** 
 * @brief   init pan gpio and timer
 * @param   void
 */
void pan_init(void) { // PE9

	__GPIOE_CLK_ENABLE();

    GPIOE->OSPEEDR |= (GPIO_SPEED_FAST << 2);   //Set fast speed.
	GPIOE->PUPDR &= ~(0x03 << (9 * 2));         //Clear bits for no push/pull
	GPIOE->MODER &= ~(0x03 << (9 * 2));         //Clear bits
	GPIOE->MODER |= (GPIO_MODE_AF_PP << (9 * 2)); 
        //Set Alternate Function Push Pull Mode
	GPIOE->AFR[1] &= ~((0x0F) << (1 * 4));
        //Clear Alternate Function for pin (lower ARF register)
	GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << (1 * 4));
        //Set Alternate Function for pin (lower ARF register) 

	// Timer 1 clock enable
	__TIM1_CLK_ENABLE();

	/* Compute the prescaler value
	   Use PSC to reduce 16Mhz sysclk down to 100kHz
	   SystemCoreClock is the system clock frequency = 16Mhz */
	TIM1->PSC = (SystemCoreClock / TIMER_FREQ) - 1;
	// Counting direction: 0 = up-counting, 1 = down-counting
	TIM1->CR1 &= ~TIM_CR1_DIR; 
	TIM1->ARR = PWM_PERIOD_TICKS - 1; 		//Set pulse period to 20ms 
    pan_write(0);
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); 	// Clear OC1M (Channel 1) 
    // Enable PWM Mode 1, upcounting, on Channel 1
	TIM1->CCMR1 |= (0x6 << TIM_CCMR1_OC1M_Pos); 
	TIM1->CCMR1 |= (TIM_CCMR1_OC1PE);// Enable output preload bit for channel 1
	TIM1->CR1  |= (TIM_CR1_ARPE);   // Set Auto-Reload Preload Enable 
	TIM1->CCER |= TIM_CCER_CC1E;    // Set CC1E Bit
	TIM1->CCER &= ~TIM_CCER_CC1NE;  // Clear CC1NE Bit for active high output
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 
	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
    pan_write(0);
}

/** 
 * @brief   set pan angle
 * @param   angle
 */
void pan_write(int angle) {

    int tick = -200 * (angle + S4595664_REG_PANTILT_PAN_ZERO_CAL_OFFSET) / 180
            + 145;
    if (tick > PWM_MAX_WIDTH) {
        tick = PWM_MAX_WIDTH; // hardcode to -85.5 deg for angle beyond limit
    } else if (tick < PWM_MIN_WIDTH) {
        tick = PWM_MIN_WIDTH;
    }
    //PWM_SET_PULSE_WIDTH_TICKS_PAN(tick);
    TIM1->CCR1 = (uint16_t) tick;
}

/** 
 * @brief   init tilt gpio and timer
 * @param   void
 */
void tilt_init(void) { // PE11

	__GPIOE_CLK_ENABLE();

    GPIOE->OSPEEDR |= (GPIO_SPEED_FAST << 2);	//Set fast speed.
	GPIOE->PUPDR &= ~(0x03 << (11 * 2));		//Clear bits for no push/pull
	GPIOE->MODER &= ~(0x03 << (11 * 2));		//Clear bits
	GPIOE->MODER |= (GPIO_MODE_AF_PP << (11 * 2));
		//Set Alternate Function Push Pull Mode
	GPIOE->AFR[1] &= ~((0x0F) << (3 * 4));
		//Clear Alternate Function for pin (lower ARF register)
	GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << (3 * 4));
		//Set Alternate Function for pin (lower ARF register) 

	// Timer 1 clock enable
	__TIM1_CLK_ENABLE();

	/* Compute the prescaler value
	   Use PSC to reduce 16Mhz sysclk down to 100kHz
	   SystemCoreClock is the system clock frequency = 16Mhz */
	TIM1->PSC = (SystemCoreClock / TIMER_FREQ) - 1;
	// Counting direction: 0 = up-counting, 1 = down-counting 
	TIM1->CR1 &= ~TIM_CR1_DIR; 
	TIM1->ARR = PWM_PERIOD_TICKS - 1; 		//Set pulse period to 20ms 
    tilt_write(0);
	//PWM_SET_PULSE_WIDTH_TICKS_TILT(220);
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC2M); 	// Clear OC1M (Channel 2) 
	// Enable PWM Mode 1, upcounting, on Channel 2
    TIM1->CCMR1 |= (0x6 << TIM_CCMR1_OC2M_Pos); 
	TIM1->CCMR1 |= (TIM_CCMR1_OC2PE); //Enable output preload bit for channel 2
	TIM1->CR1  |= (TIM_CR1_ARPE); 	// Set Auto-Reload Preload Enable 
	TIM1->CCER |= TIM_CCER_CC2E; 	// Set CC2E Bit
	TIM1->CCER &= ~TIM_CCER_CC2NE; 	// Clear CC2NE Bit for active high output
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 
	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
    tilt_write(0);
}

/** 
 * @brief   set tilt angle
 * @param   angle
 */
void tilt_write(int angle) {

    int tick = -200 * (angle + S4595664_REG_PANTILT_TILT_ZERO_CAL_OFFSET) / 180
            + 145;
    if (tick > PWM_MAX_WIDTH) {
        tick = PWM_MAX_WIDTH; 
			// tilt servo mechanically cannot bend as much as pan
    } else if (tick < PWM_MIN_WIDTH) {
        tick = PWM_MIN_WIDTH;
    }
    TIM1->CCR2 = (uint16_t) tick;
}
