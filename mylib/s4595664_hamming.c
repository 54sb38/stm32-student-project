 /** 
 **************************************************************
 * @file mylib/s4595664_hamming.c
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

#include "s4595664_hamming.h"

/**
  * Implement Hamming Code + parity checking
  * Hamming code is based on the following generator and parity check matrices
  * G = [ 0 1 1 | 1 0 0 0 ;
  *       1 0 1 | 0 1 0 0 ;
  *       1 1 0 | 0 0 1 0 ;
  *       1 1 1 | 0 0 0 1 ;
  *
  * hence H =
  * [ 1 0 0 | 0 1 1 1 ;
  *   0 1 0 | 1 0 1 1 ;
  *   0 0 1 | 1 1 0 1 ];
  *
  * y = x * G, syn = H * y'
  *
  *
  * NOTE: !! is used to get 1 out of non zeros
  */
unsigned char hamming_hbyte_encode(unsigned char value) 
{
	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits */
	d0 = !!(value & 0x1);
	d1 = !!(value & 0x2);
	d2 = !!(value & 0x4);
	d3 = !!(value & 0x8);

	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		  (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z < 8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;

    // out = (d3 d2 d1 d0 h2 h1 h0 p0)
	return out;
}

/**
 * @brief A wrapper fn for byte encode
 * 
 * @param value 
 * @return unsigned short 
 */
unsigned short s4595664_lib_hamming_byte_encode(unsigned char value)
{
    return hamming_hbyte_encode(value);
}

/**
 * @brief Decodes the hamming value.
 * 
 * @param value 8-bit encoded value with (8,4)
 *        value = (d3 d2 d1 d0 h2 h1 h0 p0)
 * @return unsigned char origional 4-bit value
 */
unsigned char s4595664_lib_hamming_byte_decode(unsigned char value)
{
    uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t s0, s1, s2, s;
    uint8_t correctedValue = value;

	/* extract bits */
    p0 = !!(value & 0b1);           // p: parity bit
    h0 = !!(value & 0b10);          // h: hamming parity-check bit
    h1 = !!(value & 0b100);
    h2 = !!(value & 0b1000);
	d0 = !!(value & 0b10000);       // d: data bit
	d1 = !!(value & 0b100000);
	d2 = !!(value & 0b1000000);
	d3 = !!(value & 0b10000000);

    s2 = h0 ^ d1 ^ d2 ^ d3;
    s1 = h1 ^ d0 ^ d2 ^ d3;
    s0 = h2 ^ d0 ^ d1 ^ d3;
    s = (s2 << 2) | (s1 << 1) | (s0 << 0);

    uint8_t h_matrix[7] = {0b100, 0b010, 0b001, 0b011, 0b101, 0b110, 0b111};

    if (s != 0) { // Error occured

        for (unsigned int i = 0; i < sizeof(h_matrix); ++i) 
        {
            if (s == h_matrix[i])
            {
                correctedValue = correctedValue ^ (1 << (7 - i));
                break;
            }
        }
    }

    uint8_t out = correctedValue >> 4;
}

/**
 * @brief checks parity error
 * 
 * @param value 
 * @return int 
 */
int s4595664_lib_hamming_parity_error(unsigned char value) // even parity
{
    uint8_t p0, h0, h1, h2, d0, d1, d2, d3;

	/* extract bits */
    p0 = !!(value & 0b1);           // p: parity bit
    h0 = !!(value & 0b10);          // h: hamming parity-check bit
    h1 = !!(value & 0b100);
    h2 = !!(value & 0b1000);
	d0 = !!(value & 0b10000);       // d: data bit
	d1 = !!(value & 0b100000);
	d2 = !!(value & 0b1000000);
	d3 = !!(value & 0b10000000);

    int parityOk = ((p0) == (h0 ^ h1 ^ h2 ^ d0 ^ d1 ^ d2 ^ d3));

    return parityOk;
}

/**
 * @brief Detects err types
 * 
 * @param value 
 * @return int 
 */
int s4595664_lib_hamming_check_error(unsigned char value)
{
    uint8_t p0, h0, h1, h2, d0, d1, d2, d3;
	uint8_t s0, s1, s2, s;

	/* extract bits */
    p0 = !!(value & 0b1);           // p: parity bit
    h0 = !!(value & 0b10);          // h: hamming parity-check bit
    h1 = !!(value & 0b100);
    h2 = !!(value & 0b1000);
	d0 = !!(value & 0b10000);       // d: data bit
	d1 = !!(value & 0b100000);
	d2 = !!(value & 0b1000000);
	d3 = !!(value & 0b10000000);

    s2 = h0 ^ d1 ^ d2 ^ d3;
    s1 = h1 ^ d0 ^ d2 ^ d3;
    s0 = h2 ^ d0 ^ d1 ^ d3;
    s = (s2 << 2) | (s1 << 1) | (s0 << 0);

    bool parityOk = false;
    if (p0 == (h0 ^ h1 ^ h2 ^ d0 ^ d1 ^ d2 ^ d3))
    {
        parityOk = true;
    }

    #ifdef DEBUG
    debug_log("s = %d, parity = %d\n\r", s, (int) parityOk);
    #endif

    int errType = 2; // Other errs

    if ((s == 0) && parityOk) { //no errs

        errType = 0;
    }
    
    if ((s != 0) && !parityOk) { // 1-bit err

        errType = 1;
    }

    if ((s != 0) && parityOk) { // 2-bit err

        errType = 2;
    }

    return errType;
}
