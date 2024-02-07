 /** 
 **************************************************************
 * @file mylib/CAG_joystic.c
 * @author Jingbo Ma - 45956649
 * @date 21052022
 * @brief CAG joystick driver
 * REFERENCE: csse3010_project.pdf 
 *            stm32f429zi_reference.pdf
 *            stm32f429zi_datasheet.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_tsk_CAG_joystick_init() - Init task
 * s4595664_tsk_CAG_joystick_deInit() - delete task
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 ***************************************************************
 */

#include "CAG_joystick.h"

#define CAGJOYSTICKTASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define CAGJOYSTICKTASK_PRIORITY (tskIDLE_PRIORITY + 4)

TaskHandle_t xJoystickHandle;

void s4595664_tsk_CAG_joystick_init(void);
void s4595664_tsk_CAG_joystick_deInit(void);
void CAG_joystick(void);


void s4595664_tsk_CAG_joystick_init(void)
{
    xTaskCreate((void *) &CAG_joystick, 
                (const signed char *) "JOYSTICK", 
                CAGJOYSTICKTASK_STACK_SIZE, 
                NULL, 
                CAGJOYSTICKTASK_PRIORITY, 
                &xJoystickHandle);
}

void s4595664_tsk_CAG_joystick_deInit(void)
{
    vTaskDelete(xJoystickHandle);
}

void CAG_joystick(void)
{
    s4595664SemaphoreJoystickPb = xSemaphoreCreateBinary();

    portDISABLE_INTERRUPTS();

    s4595664_reg_joystick_pb_init();
    s4595664_reg_joystick_init();

    portENABLE_INTERRUPTS();

    bool paused = false;

    for (;;)
    {
        int joystickX = S4595664_REG_JOYSTICK_X_READ();
        int joystickY = S4595664_REG_JOYSTICK_Y_READ();
        bool running = (bool) !!(xEventGroupGetBits(simConfigsEventGroup) & 
                            ONOFFEVENTBIT);

        if (joystickX < (0xFFF * 1 / 10))
        {
            // pause
            if (running) {

                // pause
                xEventGroupClearBits(simConfigsEventGroup, ONOFFEVENTBIT);
            }

        } else if (joystickX > (0xFFF * 9 / 10)) {

            // start
            if (!running) {

                // resume
                xEventGroupSetBits(simConfigsEventGroup, ONOFFEVENTBIT);
            }
        }

        if (joystickY > (0xFFF * 9 / 10))
        {
            // 1s
            xEventGroupClearBits(simConfigsEventGroup, SLEEPT0BIT | SLEEPT1BIT);

        } else if (joystickY < (0xFFF * 1 / 10)) {

            // 10s
            xEventGroupSetBits(simConfigsEventGroup, SLEEPT0BIT | SLEEPT1BIT);

        } else if (joystickY > (0xFFF * 45 / 100) && 
                   joystickY < (0xFFF * 55 / 100))
        {
            // 2s
            xEventGroupClearBits(simConfigsEventGroup, SLEEPT1BIT);
            xEventGroupSetBits(simConfigsEventGroup, SLEEPT0BIT);

        } else if (joystickY > (0xFFF * 7 / 10) && 
                   joystickY < (0xFFF * 8 / 10))
        {
            // 5s
            xEventGroupClearBits(simConfigsEventGroup, SLEEPT0BIT);
            xEventGroupSetBits(simConfigsEventGroup, SLEEPT1BIT);
        }

        if (s4595664SemaphoreJoystickPb != NULL)
        {
            if (xSemaphoreTake(s4595664SemaphoreJoystickPb, 
                (TickType_t) 10) == pdTRUE) {

                xEventGroupSetBits(simConfigsEventGroup, CLEARGRIDBIT);
            }
        }

        vTaskDelay(100);
    }
}
