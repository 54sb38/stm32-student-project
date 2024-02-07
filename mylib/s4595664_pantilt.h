 /** 
 **************************************************************
 * @file mylib/s4595664_pantilt.h
 * @author Jingbo Ma - 45956649
 * @date 26032022
 * @brief Pantilt driver
 * REFERENCE: csse3010_mylib_reg_pantilt.pdf 
 *            stm32f429zi_reference.pdf
 *            stm32f429zi_datasheet.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_pantilt_init() - Init gpio and timer for PWM gen
 * s4595664_pantilt_angle_write() - set pantilt angle (-85 to 85 deg)
 * s4595664_pantilt_angle_read() - read current angle
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 * S4595664_REG_PANTILT_PAN_ZERO_CAL_OFFSET
 * S4595664_REG_PANTILT_TILT_ZERO_CAL_OFFSET
 ***************************************************************
 */

#ifndef S4595664_REG_PANTILT_H
#define S4595664_REG_PANTILT_H

#include "board.h"
#include "processor_hal.h"

/*
 * public defines
 */
#define S4595664_REG_PANTILT_PAN_ZERO_CAL_OFFSET -7
#define S4595664_REG_PANTILT_TILT_ZERO_CAL_OFFSET 15

// make this the offset that need to be subtracted to get 0
/*
 * private defines
 */
#define TIMER_FREQ             100000 //100000 Hz == 0.01 ms/tick
#define PWM_PERIOD_TICKS               2000    // 20ms
// 1 tick = 0.01ms. thus for a 1.45ms pulse width, input = 145
#define PWM_MIN_WIDTH   50 // 50 ticks; -85.5 deg
#define PWM_MAX_WIDTH   240 // 240 ticks; 85.5 deg

extern void s4595664_reg_pantilt_init(void);
extern void s4595664_pantilt_angle_write(int type, int angle);
extern int s4595664_pantilt_angle_read(int type);

#define S4595664_REG_PANTILT_PAN_WRITE(angle) \
        s4595664_pantilt_angle_write(0, angle)
#define S4595664_REG_PANTILT_PAN_READ() s4595664_pantilt_angle_read(0)
#define S4595664_REG_PANTILT_TILT_WRITE(angle) \
        s4595664_pantilt_angle_write(1, angle)
#define S4595664_REG_PANTILT_TILT_READ() s4595664_pantilt_angle_read(1)

#endif
