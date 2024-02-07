#include "CAG_grid.h"

void CAG_grid(void);
void s4595664_tsk_CAG_grid_init(void);

void CAG_grid(void)
{
    // BRD_init();
    // BRD_LEDInit();
    // BRD_debuguart_init();

    s4595664_reg_lta1000g_init();
    uint8_t x_cursor = 0;
    uint8_t y_cursor = 0;
    bool running = false;
    bool useGrid = true;
    BRD_LEDGreenOn();

    for (;;)
    {
        // if (s4595664SemaphoreGrid != NULL)
        // {
        //     if (xSemaphoreTake(s4595664SemaphoreGrid, 
        //                        10) == pdTRUE) {

        //         // toggle grid use
        //         useGrid = !useGrid;
        //         BRD_LEDGreenToggle();
        //         while (BRD_debuguart_getc() != '\0'); // clear rx buffer
        //         if (s4595664SemaphorePbMne != NULL) 
        //         {	// Check if semaphore exists
        //         	// Give Pb Semaphore
        //         	xSemaphoreGiveFromISR(s4595664SemaphorePbMne, NULL);
        //         }
        //     }
        // }

        if (inputModeEventGroup != NULL)
        {
            if (!xEventGroupGetBits(inputModeEventGroup))
            {
                // toggle grid use
                useGrid = true;
                BRD_LEDGreenOn();
                //while (BRD_debuguart_getc() != '\0'); // clear rx buffer

            } else {

                useGrid = false;
            }
        }

        if (useGrid) {
            running = (bool) !!(xEventGroupGetBits(simConfigsEventGroup) & 
                                ONOFFEVENTBIT);
            unsigned char recvChar = BRD_debuguart_getc();
            switch (recvChar)
            {
                case 'w': // move up

                    if (y_cursor > 0)
                    {
                        y_cursor--;
                    }
                    break;

                case 'a': // move left

                    if (x_cursor > 0)
                    {
                        x_cursor--;
                    }
                    break;

                case 's': // move down

                    if (y_cursor < (GRID_HEIGHT - 1))
                    {
                        y_cursor++;
                    }
                    break;

                case 'd': // move right

                    if (x_cursor < (GRID_WIDTH - 1))
                    {
                        x_cursor++;
                    }
                    break;

                case 'x': // select cell

                    if (!running)
                    {
                        // spawn new
                        s4595664_CAG_grid_spawn(x_cursor, y_cursor);
                    }
                    break;

                case 'z': // unselect cell

                    if (!running)
                    {
                        s4595664_CAG_grid_kill(x_cursor, y_cursor);
                    }
                    break;

                case 'p': // toggle start/stop

                    if (!running) {

                        xEventGroupSetBits(simConfigsEventGroup, ONOFFEVENTBIT);

                    } else {

                        xEventGroupClearBits(simConfigsEventGroup, ONOFFEVENTBIT);

                    }
                    //running = !running;
                    break;

                case 'o': // move to origin

                    x_cursor = 0;
                    y_cursor = 0;
                    break;

                case 'c': // clear display

                    xEventGroupSetBits(simConfigsEventGroup, CLEARGRIDBIT);

                    break;
            }
            s4595664_reg_lta1000g_write(((uint16_t) x_cursor << 4) |
                                        y_cursor);
        }
        vTaskDelay(10);
    }
}

void s4595664_tsk_CAG_grid_init(void)
{
    xTaskCreate((void *) &CAG_grid, 
                (const signed char *) "SIM", 
                CAGGRIDTASK_STACK_SIZE, 
                NULL, 
                CAGGRIDTASK_PRIORITY, 
                NULL);
}
