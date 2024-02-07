 /** 
 **************************************************************
 * @file mylib/s4595664_joystick.c
 * @author Jingbo Ma - 45956649
 * @date 21022022
 * @brief joystick driver
 * REFERENCE: stm32f429zi_reference.pdf
 *            stm324429zi_datasheet.pdf
 *            csse3010_mylib_reg_joystick_pushbutton.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_joystick_pb_init() - Initialise joystick pb GPIO and interrupts
 * s4595664_reg_joystick_pb_isr() - Joystick Pushbutton Interrupt service 
 *                                  routine
 * s4595664_reg_joystick_press_get() - Returns the value of the Joystick 
 *                                     pushbutton press counter
 * s4595664_reg_joystick_press_reset() - Reset the Joystick event counter value 
 *                                       to 0.
 * s4595664_reg_joystick_init() - Init adc1&PC0 and adc2&PC3.
 * s4595664_joystick_readxy() - Returns the joystick x or y value.
 * s4595664_tsk_joystick_init - Create Joystick FreeRTOS task.
 *************************************************************** 
 */

#include "s4595664_joystick.h"

#define THRESHOLD 20

/* Private Variables*/
// The counter of pb pressed times
static int joystick_press_counter;
// Records when was the pb previously pressed
int prevTime;

/* Internal fuctions */
void joystick_x_init(void);
void joystick_y_init(void);
void s4595664TaskJoystick(void);

/**
 * @brief Initialise joystick pb GPIO and interrupts
 * 
 */
void s4595664_reg_joystick_pb_init(void) {

    // init PA3 (pin A0) as input and enable interrupt
    // Enable GPIOA Clock
    __GPIOA_CLK_ENABLE();

    GPIOA->OSPEEDR &= ~(0x03 << 3);
    GPIOA->OSPEEDR |= (0x02 << 3);			//Set fast speed.
    GPIOA->PUPDR &= ~(0x03 << (3 * 2));		
    GPIOA->PUPDR |= (0x01 << (3 * 2));
    GPIOA->MODER &= ~(0x03 << (3 * 2));		//Clear bits for input mode

    // Enable EXTI clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    //select trigger source (port A, pin 3) on EXTICR1.
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;

    EXTI->RTSR |= EXTI_RTSR_TR3;	//enable rising dedge
    EXTI->FTSR |= EXTI_FTSR_TR3;	//enable falling edge
    EXTI->IMR |= EXTI_IMR_IM3;		//Enable external interrupt

    //Enable priority and interrupt callback. Do not set a priority lower than 5.
    HAL_NVIC_SetPriority(EXTI3_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    joystick_press_counter = 0;
    prevTime = 0;
}

/**
 * @brief Joystick pb isr
 * 
 */
void s4595664_reg_joystick_pb_isr(void) {
    
    if ((GPIOA->IDR & (1 << 3)) == 0) 
    { // falling edge
        prevTime = HAL_GetTick();
    } 
    else 
    { // rising edge
        int thisTime = HAL_GetTick();
        if ((thisTime - prevTime) > THRESHOLD) {
            ++joystick_press_counter;
            if (s4595664SemaphoreJoystickPb != NULL) 
            {	// Check if semaphore exists
            // Give Pb Semaphore
                xSemaphoreGiveFromISR(s4595664SemaphoreJoystickPb, NULL);
            }
        }
    }
}

/**
 * @brief Returns the press count
 * 
 * @return int joystick press count
 */
int s4595664_reg_joystick_press_get(void) {

    return joystick_press_counter;
}

/**
 * @brief resets joystick press count to 0
 * 
 */
void s4595664_reg_joystick_press_reset(void) {

    joystick_press_counter = 0;
}

/**
 * @brief Init adc1&PC0 and adc2&PC3
 * 
 */
void s4595664_reg_joystick_init(void) {

    joystick_x_init();
    joystick_y_init();
}

/**
 * @brief Init adc1 & PC0
 * 
 */
void joystick_x_init(void) {

    __GPIOC_CLK_ENABLE();
  
    // Initalise PC0 as an Analog input.
    GPIOC->MODER |= (0x03 << (0 * 2));      //Set bits for Analog input mode
    GPIOC->OSPEEDR &= ~(0x03<<(0 * 2));
    GPIOC->OSPEEDR |= 0x02<<(0 * 2);        // Fast speed
    GPIOC->PUPDR &= ~(0x03 << (0 * 2));

    __ADC1_CLK_ENABLE();                //Enable ADC1 clock

    //Use ADC1 (For PC0)
    AdcHandleX.Instance = (ADC_TypeDef *)(ADC1_BASE);
    AdcHandleX.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;   
    AdcHandleX.Init.Resolution            = ADC_RESOLUTION12b;
    AdcHandleX.Init.ScanConvMode          = DISABLE;
    AdcHandleX.Init.ContinuousConvMode    = DISABLE;
    AdcHandleX.Init.DiscontinuousConvMode = DISABLE;
    AdcHandleX.Init.NbrOfDiscConversion   = 0;
    AdcHandleX.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;  
    AdcHandleX.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;  
    AdcHandleX.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    AdcHandleX.Init.NbrOfConversion       = 1;
    AdcHandleX.Init.DMAContinuousRequests = DISABLE;
    AdcHandleX.Init.EOCSelection          = DISABLE;

    //Initialise ADC
    HAL_ADC_Init(&AdcHandleX);   

    // Configure ADC Channel
    //PC0 has Analog Channel 10 connected
    AdcChanConfigX.Channel = ADC_CHANNEL_10; 
    AdcChanConfigX.Rank         = 1;
    AdcChanConfigX.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    AdcChanConfigX.Offset       = 0;

    //Initialise ADC Channel
    HAL_ADC_ConfigChannel(&AdcHandleX, &AdcChanConfigX);     
}

/**
 * @brief Init adc2 & PC3
 * 
 */
void joystick_y_init(void) {
    
    __GPIOC_CLK_ENABLE();
  
    // Initalise PC0 as an Analog input.
    GPIOC->MODER |= (0x03 << (3 * 2));      //Set bits for Analog input mode
    GPIOC->OSPEEDR &= ~(0x03<<(3 * 2));
    GPIOC->OSPEEDR |= 0x02<<(3 * 2);        // Fast speed
    GPIOC->PUPDR &= ~(0x03 << (3 * 2));

    __ADC2_CLK_ENABLE();                //Enable ADC1 clock

    //Use ADC1 (For PC0)
    AdcHandleY.Instance = (ADC_TypeDef *)(ADC2_BASE);
    AdcHandleY.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;   
    AdcHandleY.Init.Resolution            = ADC_RESOLUTION12b;
    AdcHandleY.Init.ScanConvMode          = DISABLE;
    AdcHandleY.Init.ContinuousConvMode    = DISABLE;
    AdcHandleY.Init.DiscontinuousConvMode = DISABLE;
    AdcHandleY.Init.NbrOfDiscConversion   = 0;
    AdcHandleY.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;  
    AdcHandleY.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;  
    AdcHandleY.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    AdcHandleY.Init.NbrOfConversion       = 1;
    AdcHandleY.Init.DMAContinuousRequests = DISABLE;
    AdcHandleY.Init.EOCSelection          = DISABLE;

    //Initialise ADC
    HAL_ADC_Init(&AdcHandleY);   

    // Configure ADC Channel
    //PC3 has Analog Channel 13 connected
    AdcChanConfigY.Channel = ADC_CHANNEL_13; 
    AdcChanConfigY.Rank         = 1;
    AdcChanConfigY.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    AdcChanConfigY.Offset       = 0;

    //Initialise ADC Channel
    HAL_ADC_ConfigChannel(&AdcHandleY, &AdcChanConfigY);    
}

/**
 * @brief Returns the joystick x or y value
 * 
 * @param adcHandle 
 * @return int adc output data of corresponding input adchandle
 */
int s4595664_joystick_readxy(ADC_HandleTypeDef* adcHandle) {

    HAL_ADC_Start(adcHandle);
    while (HAL_ADC_PollForConversion(adcHandle, 10) != HAL_OK);
    int value = adcHandle->Instance->DR;
    HAL_ADC_Stop(adcHandle);
    return value;
}

/**
 * @brief Creates semaphore and Inits hardware
 * 
 */
void s4595664TaskJoystick(void)
{
    s4595664SemaphoreJoystickPb = xSemaphoreCreateBinary();

    portDISABLE_INTERRUPTS();

    s4595664_reg_joystick_pb_init();

    portENABLE_INTERRUPTS();

    for (;;)
    {
        vTaskDelay(1000);
    }
}

/**
 * @brief Creates joystick task.
 * 
 */
void s4595664_tsk_joystick_init(void)
{
    xTaskCreate((void *) &s4595664TaskJoystick, 
                (const signed char *) "JOYSTICK", 
                JOYSTICKTASK_STACK_SIZE, 
                NULL, 
                JOYSTICKTASK_PRIORITY, 
                NULL);
}
