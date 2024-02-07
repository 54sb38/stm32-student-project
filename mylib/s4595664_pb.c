 /** 
 **************************************************************
 * @file mylib/s4595664_pb.c
 * @author Jingbo Ma - 45956649
 * @date 21052022
 * @brief onboard pb driver
 * REFERENCE: csse3010_project.pdf 
 *            stm32f429zi_reference.pdf
 *            stm32f429zi_datasheet.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 ***************************************************************
 */

#include "s4595664_pb.h"

#define THRESHOLD 100
#define PBTASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define PBTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

#define INPUTMODEBIT 1

/* Private Variables*/
// The counter of pb pressed times
static int pb_press_counter;
// Records when was the pb previously pressed
int prevTime;

// SemaphoreHandle_t s4595664SemaphoreGrid;
// SemaphoreHandle_t s4595664SemaphorePbMne;

EventGroupHandle_t inputModeEventGroup;


void s4595664TaskPb(void);
void s4595664_tsk_pb_init(void);
void s4595664_reg_pb_init(void);
void s4595664_reg_pb_isr(uint16_t GPIO_Pin);
int s4595664_reg_pb_press_get(void);
void s4595664_reg_pb_press_reset(void);


/**
 * @brief Creates semaphore and Inits hardware
 * 
 */
void s4595664TaskPb(void)
{
    // s4595664SemaphoreGrid = xSemaphoreCreateBinary();
    // s4595664SemaphorePbMne = xSemaphoreCreateBinary();

    inputModeEventGroup = xEventGroupCreate();

    portDISABLE_INTERRUPTS();

    s4595664_reg_pb_init();

    portENABLE_INTERRUPTS();

    for (;;)
    {
        vTaskDelay(1000);
    }
}

/**
 * @brief Creates pb task.
 * 
 */
void s4595664_tsk_pb_init(void)
{
    xTaskCreate((void *) &s4595664TaskPb, 
                (const signed char *) "PB", 
                PBTASK_STACK_SIZE, 
                NULL, 
                PBTASK_PRIORITY, 
                NULL);
}

/** 
 * @brief   init onboard pb gpio and interrupt
 * @param   void
 */
void s4595664_reg_pb_init(void) 
{
    // Enable GPIO Clock
    __GPIOC_CLK_ENABLE();

    GPIOC->OSPEEDR |= (GPIO_SPEED_FAST << 13);	//Set fast speed.
    GPIOC->PUPDR &= ~(0x03 << (13 * 2));			//Clear bits for no push/pull
    GPIOC->MODER &= ~(0x03 << (13 * 2));			//Clear bits for input mode

    // Enable EXTI clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    //select trigger source (port c, pin 13) on EXTICR4.
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    EXTI->RTSR |= EXTI_RTSR_TR13;	//enable rising dedge
    EXTI->FTSR &= ~EXTI_FTSR_TR13;	//disable falling edge
    EXTI->IMR |= EXTI_IMR_IM13;		//Enable external interrupt

    //Enable priority (10) and interrupt callback. Do not set a priority lower than 5.
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    pb_press_counter = 0;
    prevTime = 0;
}

void s4595664_reg_pb_isr(uint16_t GPIO_Pin) 
{

    // Check if the pushbutton pin was pressed.
    if (GPIO_Pin == 13) {
        
        if (!(GPIOC->IDR & (1 << 13))) 
        { // falling edge
            prevTime = HAL_GetTick();
        } 
        else 
        { // rising edge
            int thisTime = HAL_GetTick();
            if ((thisTime - prevTime) > THRESHOLD) {
                ++pb_press_counter;
                // if (s4595664SemaphoreGrid != NULL) 
                // {	// Check if semaphore exists
                // 	// Give Pb Semaphore
                // 	xSemaphoreGiveFromISR(s4595664SemaphoreGrid, NULL);
                // }

                if (inputModeEventGroup != NULL)
                {
                    if (!!xEventGroupGetBitsFromISR(inputModeEventGroup))
                    { // if is zero i.e. if using grid
                        xEventGroupClearBitsFromISR(inputModeEventGroup, 1);
                    } else { // if using mne

                        xEventGroupSetBitsFromISR(inputModeEventGroup, 1, NULL);
                    }
                    
                }

            }
        }

    }
}

/**
 * @brief Returns the press count
 * 
 * @return int pb press count
 */
int s4595664_reg_pb_press_get(void) 
{
    return pb_press_counter;
}

/**
 * @brief resets pb press count to 0
 * 
 */
void s4595664_reg_pb_press_reset(void) 
{
    pb_press_counter = 0;
}
