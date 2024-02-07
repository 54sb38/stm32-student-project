 /** 
 **************************************************************
 * @file mylib/s4595664_pb.h
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

#ifndef S4595664_PB_H
#define S4595664_PB_H

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_log.h"

// extern SemaphoreHandle_t s4595664SemaphorePbMne;
// extern SemaphoreHandle_t s4595664SemaphoreGrid;

extern EventGroupHandle_t inputModeEventGroup;


extern void s4595664TaskPb(void);
extern void s4595664_tsk_pb_init(void);
extern void s4595664_reg_pb_init(void);
extern void s4595664_reg_pb_isr(uint16_t GPIO_Pin);
extern int s4595664_reg_pb_press_get(void);
extern void s4595664_reg_pb_press_reset(void);


#endif
