 /** 
 **************************************************************
 * @file mylib/s4595664_oled.h
 * @author Jingbo Ma - 45956649
 * @date 01052022
 * @brief Oled driver
 * REFERENCE: csse3010_mylib_reg_oled.pdf 
 *            stm32f429zi_reference.pdf
 *            freertos.org
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_tsk_oled_init() - Creates the Oled display task.
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 * s4595664SemaphoreOledMsg - New oled message notification semaphore
 * s4595664QueueOledMsg - Oled message data
 ***************************************************************
 */

#ifndef S4595664_OLED_H
#define S4595664_OLED_H

#include "board.h"
#include "processor_hal.h"
#include "oled_pixel.h"
#include "oled_string.h"
#include "fonts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef struct oledTextMsg
{
    int startX;
    int startY;
    char displayText[20];
    
} oledTextMsg;

#define I2C_DEV_SDA_PIN		9
#define I2C_DEV_SCL_PIN		8
#define I2C_DEV_GPIO		GPIOB
#define I2C_DEV_GPIO_AF 	GPIO_AF4_I2C1
#define I2C_DEV_GPIO_CLK()	__GPIOB_CLK_ENABLE()

#define I2C_DEV				I2C1
#define I2C_DEV_CLOCKSPEED 	100000

#define OLEDTASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define OLEDTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 4)

// global variables

extern SemaphoreHandle_t s4595664SemaphoreOledMsg;
extern QueueHandle_t s4595664QueueOledMsg;

extern void s4595664_tsk_oled_init(void);

#endif
