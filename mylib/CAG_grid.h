#ifndef CAG_GRID_H
#define CAG_GRID_H

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
#include "CAG_simulator.h"
#include "s4595664_lta1000g.h"
#include "s4595664_pb.h"

#define CAGGRIDTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define CAGGRIDTASK_PRIORITY (tskIDLE_PRIORITY + 3)

extern void s4595664_tsk_CAG_grid_init(void);

#endif
