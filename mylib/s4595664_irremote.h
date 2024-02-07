 /** 
 **************************************************************
 * @file mylib/s4595664_irremote.h
 * @author Jingbo Ma - 45956649
 * @date 09042022
 * @brief joystick driver
 * REFERENCE: stm32f429zi_reference.pdf
 *            stm324429zi_datasheet.pdf
 *            https://exploreembedded.com/wiki/NEC_IR_Remote_Control_Interface_with_8051
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_irremote_init() - Initialise the timer
 * s4595664_reg_irremote_recv() - isr callback for input capture interrupt
 * s4595664_reg_irremote_readkey() - read the latest keypress reading
 *************************************************************** 
 */

/***************************************************************************************************
                                    ExploreEmbedded Copyright Notice 
****************************************************************************************************
 * File:   necIrRemoteExample1.c
 * Version: 16.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Sample program to decode NEC IR Remote pulses and display on LCD using 8051 controller.
This code has been developed and tested on ExploreEmbedded boards.  
We strongly believe that the library works on any of development boards for respective controllers. 
Check this link http://www.exploreembedded.com/wiki for awesome tutorials on 8051,PIC,AVR,ARM,Robotics,RTOS,IOT.
ExploreEmbedded invests substantial time and effort developing open source HW and SW tools, to support consider 
buying the ExploreEmbedded boards.
 
The ExploreEmbedded libraries and examples are licensed under the terms of the new-bsd license(two-clause bsd license).
See also: http://www.opensource.org/licenses/bsd-license.php
EXPLOREEMBEDDED DISCLAIMS ANY KIND OF HARDWARE FAILURE RESULTING OUT OF USAGE OF LIBRARIES, DIRECTLY OR
INDIRECTLY. FILES MAY BE SUBJECT TO CHANGE WITHOUT PRIOR NOTICE. THE REVISION HISTORY CONTAINS THE INFORMATION 
RELATED TO UPDATES.
 
Permission to use, copy, modify, and distribute this software and its documentation for any purpose
and without fee is hereby granted, provided that this copyright notices appear in all copies 
and that both those copyright notices and this permission notice appear in supporting documentation.
**************************************************************************************************/

#ifndef S4595664_IRREMOTE_H
#define S4595664_IRREMOTE_H

#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include <stdint.h>
#include <stdbool.h>

//#define DEBUG

#define TICKSEVERYMS 16000                       // Period for 1ms

#define BITPATTERN0 0xFF6897
#define BITPATTERN1 0xFF30CF
#define BITPATTERN2 0xFF18E7
#define BITPATTERN3 0xFF7A85
#define BITPATTERN4 0xFF10EF
#define BITPATTERN5 0xFF38C7
#define BITPATTERN6 0xFF5AA5
#define BITPATTERN7 0xFF42BD
#define BITPATTERN8 0xFF4AB5
#define BITPATTERN9 0xFF52AD

int keyPressedFlag;
char keyPressedValue;

uint32_t bitPattern, newKey;
uint32_t timerValue, prevTimerValue;
int pulseCount;

extern void s4595664_reg_irremote_init();
extern void s4595664_reg_irremote_recv();
extern int s4595664_reg_irremote_readkey(char* value);

#ifdef DEBUG
extern uint32_t debug_get_newkey(void);
extern uint32_t debug_get_pattern(void);
extern uint32_t debug_get_pulse(void);
#endif

#endif
