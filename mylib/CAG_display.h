 /** 
 **************************************************************
 * @file mylib/CAG_display.h
 * @author Jingbo Ma - 45956649
 * @date 01052022
 * @brief Oled driver
 * REFERENCE: csse3010_mylib_reg_oled.pdf 
 *            stm32f429zi_reference.pdf
 *            freertos.org
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_oled_init() - Creates the Oled display task.
 * s4595664_reg_oled_setCell 
 * s4595664_reg_oled_clearCell
 * s4595664_reg_oled_clearMap
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 * s4595664SemaphoreUpdateOled - New oled message notification semaphore
 * s4595664MutexOled - Oled mutex
 ***************************************************************
 */

#ifndef CAG_DISPLAY_H
#define CAG_DISPLAY_H

#include <stdint.h>
#include "board.h"
#include "processor_hal.h"
#include "oled_pixel.h"
#include "oled_string.h"
#include "fonts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//#include "CAG_simulator.h"


#define I2C_DEV_SDA_PIN		9
#define I2C_DEV_SCL_PIN		8
#define I2C_DEV_GPIO		GPIOB
#define I2C_DEV_GPIO_AF 	GPIO_AF4_I2C1
#define I2C_DEV_GPIO_CLK()	__GPIOB_CLK_ENABLE()

#define I2C_DEV				I2C1
#define I2C_DEV_CLOCKSPEED 	100000

#define OLEDTASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define OLEDTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 16)

// global variables

extern SemaphoreHandle_t s4595664SemaphoreUpdateOled;
extern SemaphoreHandle_t s4595664MutexOled;

extern void s4595664_tsk_oled_init(void);
extern void s4595664_reg_oled_setCell(uint8_t x, uint8_t y);
extern void s4595664_reg_oled_clearCell(uint8_t x, uint8_t y);
extern void s4595664_reg_oled_clearMap(void);

#endif
