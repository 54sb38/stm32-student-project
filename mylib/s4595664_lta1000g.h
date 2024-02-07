 /** 
 **************************************************************
 * @file mylib/s4595664_lta1000g.h
 * @author Jingbo Ma - 45956649
 * @date 21022022
 * @brief mylib lta1000g driver
 * REFERENCE: stm32f429zi_reference.pdf 
 *            csse3010_mylib_reg_lta1000g.pdf  
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_lta1000g_init() - Initialise LED Bar GPIO pins as outputs.
 * s4595664_reg_lta1000g_write() - Write value to disp on LED bar
 *************************************************************** 
 */

#ifndef S4595664_LTA1000G_H
#define S4595664_LTA1000G_H

#include "board.h"
#include "processor_hal.h"

extern void s4595664_reg_lta1000g_init(void);
extern void s4595664_reg_lta1000g_write(unsigned short value);

void lta1000g_seg_set(int segment, unsigned char segment_value);

#endif
