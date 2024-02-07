/**
 ******************************************************************************
 * @file    main.h
 * @author  Jingbo Ma
 * @date    01052022
 * @brief   S4
 ******************************************************************************
 *
 */

#include "main.h"


/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void ) {

    HAL_Init();	

    //debug_log("main\n\r");

    s4595664_tsk_pb_init();
    s4595664_tsk_CAG_simulator_init();
    s4595664_tsk_oled_init();
    s4595664_tsk_CAG_grid_init();
    s4595664_CAG_mnemonic_init();
    s4595664_tsk_CAG_joystick_init();

    vTaskStartScheduler();

    return 0;
}
