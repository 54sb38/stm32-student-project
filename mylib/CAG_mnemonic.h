 /** 
 **************************************************************
 * @file mylib/CAG_mnemonic.h
 * @author Jingbo Ma - 45956649
 * @date 21052022
 * @brief CAG joystick driver
 * REFERENCE: csse3010_project.pdf 
 *            stm32f429zi_reference.pdf
 *            stm32f429zi_datasheet.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_CAG_mnemonic_init() - Init task
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 ***************************************************************
 */

#ifndef CAG_MNEMONIC_H
#define CAG_MNEMONIC_H

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_CLI.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_log.h"
#include "CAG_simulator.h"
#include "CAG_joystick.h"
#include "s4595664_pb.h"


extern void s4595664_CAG_mnemonic_init(void);


#endif
