/**
 ******************************************************************************
 * @file    it_handler.c
 * @author  MDS
 * @date    02022021
 * @brief   Must only be used to for interrupt handlers. Only callbacks must be
 *          called in this file. No other code must be present.
 ******************************************************************************
 *
 */

#include "main.h"
#include "s4595664_joystick.h"
#include "s4595664_pb.h"

/*
 * Interrupt handler for EXTI 3 IRQ Handler
 */ 
void EXTI3_IRQHandler(void) {

    NVIC_ClearPendingIRQ(EXTI3_IRQn);

    // PR: Pending register
    if ((EXTI->PR & EXTI_PR_PR3) == EXTI_PR_PR3) {

        s4595664_reg_joystick_pb_isr();
        // cleared by writing a 1 to this bit
        EXTI->PR |= EXTI_PR_PR3;	//Clear interrupt flag.        
    }	
}

void EXTI15_10_IRQHandler(void) {

	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

	// PR: Pending register
	if ((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13) {

		s4595664_reg_pb_isr(13);   // Callback for C13

		// cleared by writing a 1 to this bit
		EXTI->PR |= EXTI_PR_PR13;	//Clear interrupt flag.

	}
}
