 /** 
 **************************************************************
 * @file mylib/s4595664_irremote.c
 * @author Jingbo Ma - 45956649
 * @date 09042022
 * @brief joystick driver
 * REFERENCE: stm32f429zi_reference.pdf
 *            stm324429zi_datasheet.pdf
 *            https://exploreembedded.com/wiki/NEC_IR_Remote_Control_Interface_with_8051
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_irremote_init() - Initialise the timer
 * s4595664_reg_irremote_recv() - isr callback for input capture interrupt
 * s4595664_reg_irremote_readkey() - read the latest keypress reading
 *************************************************************** 
 */

#include "s4595664_irremote.h"

void s4595664_reg_irremote_init()
{
    BRD_LEDInit();
    // Enable GPIOB Clock
    __GPIOB_CLK_ENABLE();

    //Set fast speed.
    GPIOB->OSPEEDR |= (GPIO_SPEED_FAST << 10);
    //Clear bits for no push/pull	
    GPIOB->PUPDR &= ~(0x03 << (10 * 2));
    //Clear bits
    GPIOB->MODER &= ~(0x03 << (10 * 2));
    //Set Alternate Function Push Pull Mode		
    GPIOB->MODER |= (GPIO_MODE_AF_PP << (10 * 2));
    //Clear Alternate Function for pin (higher ARF register)
    GPIOB->AFR[1] &= ~((0x0F) << (2 * 4));
    //Set Alternate Function for pin (higher ARF register)
    GPIOB->AFR[1] |= (GPIO_AF1_TIM2 << (2 * 4)); 

    // Timer 2 clock enable
    __TIM2_CLK_ENABLE();

    // Compute the prescaler value to set the timer counting frequency to 1kHz
    // SystemCoreClock is the system clock frequency
    TIM2->PSC = 0; // this does nothing

    // Counting direction: 0 = up-counting (Timer Control Register 1)
    TIM2->CR1 &= ~TIM_CR1_DIR; 

    // Set the direction as input and select the active input
    // CC1S[1:0] for channel 1;
    // 00 = output
    // 01 = input, CC1 is mapped on timer Input 1
    // 10 = input, CC1 is mapped on timer Input 2
    // 11 = input, CC1 is mapped on slave timer
    TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
    TIM2->CCMR2 |= TIM_CCMR2_CC3S_0; 
                                            
    // Disable digital filtering by clearing IC1F[3:0] bits
    // because we want to capture every event
    TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
    
    // Select the edge of the active transition
    // Detect only rising edges
    // CC3NP:CC3P bits
    // 00 = rising edge,
    // 01 = falling edge,
    // 10 = reserved,
    // 11 = both edges
    TIM2->CCER &= ~(1U << 11);
    TIM2->CCER |= (1U << 9);		//falling edge
    
    // Program the input prescaler
    // To capture each valid transition, set the input prescaler to zero;
    // IC3PSC[1:0] bits (input capture 1 prescaler)
    // Clear filtering because we need to capture every event
    TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC; 
    
    // Enable Capture for channel 3
    TIM2->CCER |= TIM_CCER_CC3E;    
    
    // Enable related interrupts
    // Enable Capture interrupts for channel 3
    TIM2->DIER |= TIM_DIER_CC3IE;
    // Enable update interrupt to occur on timer counter overflow or underflow.
    TIM2->DIER |= TIM_DIER_UIE;	
    
    // Enable priority (10) and interrupt callback. 
    // Do not set a priority lower than 5.
    HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;				// Enable the counter

    keyPressedFlag = 0;
    keyPressedValue = 0;
    bitPattern = 0;
    newKey = 0;
    timerValue = 0;
    prevTimerValue = 0;
    pulseCount = 0;

}

void s4595664_reg_irremote_recv()
{
    // Check if overlfow has taken place
    if((TIM2->SR & TIM_SR_UIF) != TIM_SR_UIF) {	 

        TIM2->SR &= ~TIM_SR_UIF; 	// Clear the UIF Flag
    }

    // Check if input capture has taken place 
    if((TIM2->SR & TIM_SR_CC3IF) == TIM_SR_CC3IF) { 

        timerValue = TIM2->CCR3;  // Reading CCR3 clears CC3IF
        TIM2->CNT = 0;  // reset counter reg

        pulseCount++;

        // If the pulse width is greater than 50ms, this will mark the SOF
        if (timerValue >= (50 * TICKSEVERYMS)) 
        {   
            BRD_LEDBlueOn();
            BRD_LEDRedOff();
            BRD_LEDGreenOff();
            // First 2 counts needs to be skipped hence pulse count is set to -2 
            pulseCount = -2; 
            bitPattern = 0;
        }
        //Accumulate the bit values between 0-31.
        else if ((pulseCount >= 0) && (pulseCount < 32)) 
        {   
            BRD_LEDRedOn();
            BRD_LEDBlueOff();
            BRD_LEDGreenOff();
            if(timerValue >= (2 * TICKSEVERYMS))
            //pulse width greater than 2ms is considered as LOGIC1
            {
                bitPattern |= (uint32_t) 1 << (31 - pulseCount);
            }
            else
            {
                // LOGIC0
            }
        }
        //This will mark the End of frame as 32 pulses are received
        if (pulseCount >= 31)
        {
            BRD_LEDGreenOn();
            BRD_LEDRedOff();
            BRD_LEDBlueOff();
            newKey = bitPattern;
            pulseCount = 0;
            keyPressedFlag = 1;
            keyPressedValue = 'N';
            switch (newKey)
            {
                case BITPATTERN0:

                    keyPressedValue = '0';
                    break;

                case BITPATTERN1:

                    keyPressedValue = '1';
                    break;

                case BITPATTERN2:

                    keyPressedValue = '2';
                    break;

                case BITPATTERN3:

                    keyPressedValue = '3';
                    break;

                case BITPATTERN4:

                    keyPressedValue = '4';
                    break;
                    
                case BITPATTERN5:

                    keyPressedValue = '5';
                    break;

                case BITPATTERN6:

                    keyPressedValue = '6';
                    break;

                case BITPATTERN7:

                    keyPressedValue = '7';
                    break;

                case BITPATTERN8:

                    keyPressedValue = '8';
                    break;
                    
                case BITPATTERN9:

                    keyPressedValue = '9';
                    break;
            }
        }
    }
}

#ifdef DEBUG
uint32_t debug_get_newkey(void)
{
    return newKey;
}

uint32_t debug_get_pattern(void)
{
    return bitPattern;
}

uint32_t debug_get_pulse(void)
{
    return pulseCount;
}
#endif

/*************************
  NEC IR Remote Codes 
**************************  
0xFFA25D: CH-       
0xFF629D: CH        
0xFFE21D: CH+       
0xFF22DD: PREV      
0xFF02FD: NEXT      
0xFFC23D: PLAY/PAUSE
0xFFE01F: VOL-      
0xFFA857: VOL+      
0xFF906F: EQ        
0xFF6897: 0         
0xFF9867: 100+      
0xFFB04F: 200+      
0xFF30CF: 1         
0xFF18E7: 2         
0xFF7A85: 3         
0xFF10EF: 4         
0xFF38C7: 5         
0xFF5AA5: 6         
0xFF42BD: 7         
0xFF4AB5: 8         
0xFF52AD: 9 
**************************/

int s4595664_reg_irremote_readkey(char* value)
{
    if (keyPressedFlag == 1)
    {
        *value = keyPressedValue;
        keyPressedFlag = 0;

        return 1;
    }

    return 0;
}
