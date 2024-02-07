 /** 
 **************************************************************
 * @file mylib/s4595664_joystick.h
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

#ifndef S4595664_JOYSTICK_H
#define S4595664_JOYSTICK_H

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
// #include "queue.h"
#include "semphr.h"

// ADC handles
ADC_HandleTypeDef AdcHandleX;
ADC_ChannelConfTypeDef AdcChanConfigX;
ADC_HandleTypeDef AdcHandleY;
ADC_ChannelConfTypeDef AdcChanConfigY;

// Global variables
SemaphoreHandle_t s4595664SemaphoreJoystickPb;

extern SemaphoreHandle_t s4595664SemaphoreJoystickPb;

extern void s4595664_reg_joystick_pb_init(void);
extern void s4595664_reg_joystick_pb_isr(void);
extern int s4595664_reg_joystick_press_get(void);
extern void s4595664_reg_joystick_press_reset(void);
extern void s4595664_reg_joystick_init(void);
extern int s4595664_joystick_readxy(ADC_HandleTypeDef* adcHandle);
extern void s4595664_tsk_joystick_init(void);

#define S4595664_REG_JOYSTICK_X_READ() \
        s4595664_joystick_readxy(&AdcHandleX) \
        + S4595664_REG_JOYSTICK_X_ZERO_CAL_OFFSET
#define S4595664_REG_JOYSTICK_Y_READ() \
        s4595664_joystick_readxy(&AdcHandleY) \
        + S4595664_REG_JOYSTICK_Y_ZERO_CAL_OFFSET

#define S4595664_REG_JOYSTICK_X_ZERO_CAL_OFFSET 0
#define S4595664_REG_JOYSTICK_Y_ZERO_CAL_OFFSET 0

#define JOYSTICKTASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define JOYSTICKTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

#endif
