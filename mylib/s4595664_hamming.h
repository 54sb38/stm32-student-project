 /** 
 **************************************************************
 * @file mylib/s4595664_hamming.h
 * @author Jingbo Ma - 45956649
 * @date 09042022
 * @brief joystick driver
 * REFERENCE: stm32f429zi_reference.pdf
 *            stm324429zi_datasheet.pdf
 *            sourcelib/example/geting_started/hamming
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_lib_hamming_byte_encode() - Return 16bit encoded value of a byte
 * s4595664_lib_hamming_byte_decode() - Return the decoded half byte from a byte
 * s4595664_lib_hamming_parity_error() - Return 1 if a parity err has occured,
 *                                     else return 0.
 * s4595664_lib_hamming_check_error() - Return 0 for no err,
 *                                      return 1 for 1-bit err
 *                                      return 2 for 2-bit error and others
 *************************************************************** 
 */

#ifndef HAMMING_H
#define HAMMING_H

#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include <stdint.h>
#include <stdbool.h>

//#define DEBUG

/* External functions */
extern unsigned short s4595664_lib_hamming_byte_encode(unsigned char value);
extern unsigned char s4595664_lib_hamming_byte_decode(unsigned char value);
extern int s4595664_lib_hamming_parity_error(unsigned char value);
extern int s4595664_lib_hamming_check_error(unsigned char value);

/* Internal functions */
unsigned char hamming_hbyte_encode(unsigned char value);


#endif
