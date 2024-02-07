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
 * s4595664_tsk_CAG_simulator_init();
 * s4595664_CAG_clear_grid();
 * s4595664_CAG_grid_spawn();
 * s4595664_CAG_grid_kill();
 * s4595664_tsk_CAG_simulator_deInit();
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 ***************************************************************
 */

#ifndef CAG_SIMULATOR_H
#define CAG_SIMULATOR_H

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
#include "CAG_display.h"
#include "debug_log.h"

#define GRID_WIDTH 64
#define GRID_HEIGHT 16

#define CAGSIMTASK_PRIORITY (tskIDLE_PRIORITY + 4)
#define CAGSIMTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 16)

#define ONOFFEVENTBIT 1
#define SLEEPTIMEBITS 0b110
#define CLEARGRIDBIT (1 << 4)
#define SLEEPT0BIT (1 << 1)
#define SLEEPT1BIT (1 << 2)

#define DEAD 0
#define ALIVE 1
#define BLOCK 0
#define BEEHIVE 1
#define LOAF 2
#define BLINKER 0
#define TOAD 1
#define BEACON 2

#define STILL 2
#define OSC 3
#define GLIDER 4
#define CELL 1

typedef struct cell 
{
    bool alive;
    uint32_t stateValue;

} Cell;

typedef struct caMessage
{
    int type;
    int cell_x; // cell/lf x position
    int cell_y; // cell/lf y position

} caMessage_t; 

extern EventGroupHandle_t simConfigsEventGroup;
extern SemaphoreHandle_t s4595664MutexGrid;
extern SemaphoreHandle_t s4595664SpawnPattern;
extern QueueHandle_t s4595664CaMessageQueue;


extern void s4595664_tsk_CAG_simulator_init(void);
extern void s4595664_CAG_clear_grid(void);
extern void s4595664_CAG_grid_spawn(int8_t x, int8_t y);
extern void s4595664_CAG_grid_kill(int8_t x, int8_t y);
extern void s4595664_tsk_CAG_simulator_deInit(void);


#endif
