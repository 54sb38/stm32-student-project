 /** 
 **************************************************************
 * @file mylib/s4595664_lta1000g.cWrite the LED Bar segments high or low
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

#include "s4595664_lta1000g.h"

/**
 * @brief Initialise LED Bar GPIO pins as outputs.
 * 
 */
void s4595664_reg_lta1000g_init(void) {
    
    // Enable the GPIO A Clock
    __GPIOA_CLK_ENABLE();
    //Initialise A4, A15 as an output.
    GPIOA->MODER &= ~((0x03 << 4 * 2) | (0x03 << 15 * 2));  //clear bits
    GPIOA->MODER |= ((0x01 << 4 * 2) | (0x01 << 15 * 2));   //Set for output
    // Set for Fast speed
    GPIOA->OSPEEDR &= ~((0x03 << 4 * 2) | (0x03 << 15 * 2));
    GPIOA->OSPEEDR |= ((0x02 << 4 * 2) | (0x02 << 15 * 2)); 
    //Clear Bit for Push/Pull
    GPIOA->OTYPER &= ~((0x01 << 4) | (0x01 << 15)); 
    //Set pull-down
    GPIOA->PUPDR &= ~((0x03 << 4 * 2) | (0x03 << 15 * 2));  
    GPIOA->PUPDR |= ((0x2 << 4 * 2) | (0x2 << 15 * 2));

    // Enable the GPIO B Clock
    __GPIOB_CLK_ENABLE();
    //Initialise B3, B4, B5, B12, B13, B15 as an output.
    GPIOB->MODER &= ~((0x03 << 3 * 2) | (0x03 << 4 * 2) | (0x03 << 5 * 2)
            | (0x03 << 12 * 2) | (0x03 << 13 * 2) | (0x03 << 15 * 2)); 
    GPIOB->MODER |= ((0x01 << 3 * 2) | (0x01 << 4 * 2) | (0x01 << 5 * 2)
            | (0x01 << 12 * 2) | (0x01 << 13 * 2) | (0x01 << 15 * 2));

    GPIOB->OSPEEDR &= ~((0x03 << 3 * 2) | (0x03 << 4 * 2) | (0x03 << 5 * 2)
            | (0x03 << 12 * 2) | (0x03 << 13 * 2) | (0x03 << 15 * 2));
    GPIOB->OSPEEDR |= ((0x02 << 3 * 2) | (0x02 << 4 * 2) | (0x02 << 5 * 2)
            | (0x02 << 12 * 2) | (0x02 << 13 * 2) | (0x02 << 15 * 2));

    //Clear Bit for Push/Pull output
    GPIOB->OTYPER &= ~((0x01 << 3) | (0x01 << 4) | (0x01 << 5) | (0x01 << 12) 
            | (0x01 << 13) | (0x01 << 15));

    GPIOB->PUPDR &= ~((0x03 << 3 * 2) | (0x03 << 4 * 2) | (0x03 << 5 * 2)
            | (0x03 << 12 * 2) | (0x03 << 13 * 2) | (0x03 << 15 * 2));
    GPIOB->PUPDR |= ((0x02 << 3 * 2) | (0x02 << 4 * 2) | (0x02 << 5 * 2)
            | (0x02 << 12 * 2) | (0x02 << 13 * 2) | (0x02 << 15 * 2));

    // Enable the GPIO C Clock
    __GPIOC_CLK_ENABLE();
    //Initialise C6, C7 as an output.
    GPIOC->MODER &= ~((0x03 << 6 * 2) | (0x03 << 7 * 2));  //clear bits
    GPIOC->MODER |= ((0x01 << 6 * 2) | (0x01 << 7 * 2));   //Set for output

    GPIOC->OSPEEDR &= ~((0x03 << 6 * 2) | (0x03 << 7 * 2));
    GPIOC->OSPEEDR |= ((0x02 << 6 * 2) | (0x02 << 7 * 2)); 

    GPIOC->OTYPER &= ~((0x01 << 6) | (0x01 << 7)); 

    GPIOC->PUPDR &= ~((0x03 << 6 * 2) | (0x03 << 7 * 2));
    GPIOC->PUPDR |= ((0x02 << 6 * 2) | (0x02 << 7 * 2));  
}

/**
 * @brief set one segment on the LED bar
 * 
 * @param segment led bar segment (0-9)
 * @param segment_value on/off of LED (0-1)
 */
void lta1000g_seg_set(int segment, 
                 unsigned char segment_value) {
                     
    if (segment_value == 0) {

        switch (segment) {

            case 0:
                GPIOC->BSRR &= ~(0x01 << 6);
                GPIOC->BSRR |= (0x01 << (6 + 16));
                break;

            case 1:
                GPIOB->BSRR &= ~(0x01 << 15);
                GPIOB->BSRR |= (0x01 << (15 + 16));
                break;

            case 2:
                GPIOB->BSRR &= ~(0x01 << 13);
                GPIOB->BSRR |= (0x01 << (13 + 16));
                break;

            case 3:
                GPIOB->BSRR &= ~(0x01 << 12);
                GPIOB->BSRR |= (0x01 << (12 + 16));
                break;

            case 4:
                GPIOA->BSRR &= ~(0x01 << 15);
                GPIOA->BSRR |= (0x01 << (15 + 16));
                break;

            case 5:
                GPIOC->BSRR &= ~(0x01 << 7);
                GPIOC->BSRR |= (0x01 << (7 + 16));
                break;

            case 6:
                GPIOB->BSRR &= ~(0x01 << 5);
                GPIOB->BSRR |= (0x01 << (5 + 16));
                break;

            case 7:
                GPIOB->BSRR &= ~(0x01 << 3);
                GPIOB->BSRR |= (0x01 << (3 + 16));
                break;

            case 8:
                GPIOA->BSRR &= ~(0x01 << 4);
                GPIOA->BSRR |= (0x01 << (4 + 16));
                break;

            case 9:
                GPIOB->BSRR &= ~(0x01 << 4);
                GPIOB->BSRR |= (0x01 << (4 + 16));
                break;

        }
    } else if (segment_value == 1) {
        
        switch (segment) {
            case 0:
                GPIOC->BSRR |= (0x01 << 6);
                break;

            case 1:
                GPIOB->BSRR |= (0x01 << 15);
                break;

            case 2:
                GPIOB->BSRR |= (0x01 << 13);
                break;

            case 3:
                GPIOB->BSRR |= (0x01 << 12);
                break;

            case 4:
                GPIOA->BSRR |= (0x01 << 15);
                break;

            case 5:
                GPIOC->BSRR |= (0x01 << 7);
                break;

            case 6:
                GPIOB->BSRR |= (0x01 << 5);
                break;

            case 7:
                GPIOB->BSRR |= (0x01 << 3);
                break;

            case 8:
                GPIOA->BSRR |= (0x01 << 4);
                break;

            case 9:
                GPIOB->BSRR |= (0x01 << 4);
                break;

        }
    }
}

/**
 * @brief Write value to disp on LED bar
 * 
 * @param value value to display on LED bar 
 */
void s4595664_reg_lta1000g_write(unsigned short value) {

    for (int segment = 0; segment < 10; ++segment) {

        unsigned char seg_value = 0;

        if (value & (1 << segment)) { 

            seg_value = 1;
        }

        lta1000g_seg_set(segment, seg_value);
    }
}
