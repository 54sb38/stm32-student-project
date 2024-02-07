 /** 
 **************************************************************
 * @file mylib/CAG_display.c
 * @author Jingbo Ma - 45956649
 * @date 01052022
 * @brief Oled driver
 * REFERENCE: csse3010_mylib_reg_oled.pdf 
 *            stm32f429zi_reference.pdf
 *            freertos.org
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_reg_oled_init() - Creates the Oled display task.
 * s4595664_reg_oled_setCell 
 * s4595664_reg_oled_clearCell
 * s4595664_reg_oled_clearMap
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 * s4595664SemaphoreUpdateOled - New oled message notification semaphore
 * s4595664MutexOled - Oled mutex
 ***************************************************************
 */

#include "CAG_display.h"

SemaphoreHandle_t s4595664SemaphoreUpdateOled;
SemaphoreHandle_t s4595664MutexOled;
//SemaphoreHandle_t s4595664SemaphoreStartSim;

// Internal functions
void s4595664TaskOled(void);
//void oled_write(oledTextMsg);
void s4595664_reg_oled_init(void);
void s4595664_reg_oled_setCell(uint8_t x, uint8_t y);
void s4595664_reg_oled_clearCell(uint8_t x, uint8_t y);
void s4595664_reg_oled_clearMap(void);


/**
 * @brief Inits the GPIOs and set up i2c.
 * 
 */
void s4595664_reg_oled_init(void) 
{
    uint32_t pclk1;
    uint32_t freqrange;

    // Enable GPIO clock
    I2C_DEV_GPIO_CLK();

    //******************************************************
    // IMPORTANT NOTE: SCL Must be Initialised BEFORE SDA
    //******************************************************

    //Clear and Set Alternate Function for pin (lower ARF register) 
    MODIFY_REG(I2C_DEV_GPIO->AFR[1], ((0x0F) << ((I2C_DEV_SCL_PIN-8) * 4)) | ((0x0F) << ((I2C_DEV_SDA_PIN-8)* 4)), ((I2C_DEV_GPIO_AF << ((I2C_DEV_SCL_PIN-8) * 4)) | (I2C_DEV_GPIO_AF << ((I2C_DEV_SDA_PIN-8)) * 4)));
    
    //Clear and Set Alternate Function Push Pull Mode
    MODIFY_REG(I2C_DEV_GPIO->MODER, ((0x03 << (I2C_DEV_SCL_PIN * 2)) | (0x03 << (I2C_DEV_SDA_PIN * 2))), ((GPIO_MODE_AF_OD << (I2C_DEV_SCL_PIN * 2)) | (GPIO_MODE_AF_OD << (I2C_DEV_SDA_PIN * 2))));
    
    //Set low speed.
    SET_BIT(I2C_DEV_GPIO->OSPEEDR, (GPIO_SPEED_LOW << I2C_DEV_SCL_PIN) | (GPIO_SPEED_LOW << I2C_DEV_SDA_PIN));

    //Set Bit for Push/Pull output
    SET_BIT(I2C_DEV_GPIO->OTYPER, ((0x01 << I2C_DEV_SCL_PIN) | (0x01 << I2C_DEV_SDA_PIN)));

    //Clear and set bits for no push/pull
    MODIFY_REG(I2C_DEV_GPIO->PUPDR, (0x03 << (I2C_DEV_SCL_PIN * 2)) | (0x03 << (I2C_DEV_SDA_PIN * 2)), (GPIO_PULLUP << (I2C_DEV_SCL_PIN * 2)) | (GPIO_PULLUP << (I2C_DEV_SDA_PIN * 2)));

    // Configure the I2C peripheral
    // Enable I2C peripheral clock
    __I2C1_CLK_ENABLE();

    // Disable the selected I2C peripheral
    CLEAR_BIT(I2C_DEV->CR1, I2C_CR1_PE);

      pclk1 = HAL_RCC_GetPCLK1Freq();			// Get PCLK1 frequency
      freqrange = I2C_FREQRANGE(pclk1);		// Calculate frequency range 

      //I2Cx CR2 Configuration - Configure I2Cx: Frequency range
      MODIFY_REG(I2C_DEV->CR2, I2C_CR2_FREQ, freqrange);

    // I2Cx TRISE Configuration - Configure I2Cx: Rise Time
      MODIFY_REG(I2C_DEV->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, I2C_DEV_CLOCKSPEED));

       // I2Cx CCR Configuration - Configure I2Cx: Speed
      MODIFY_REG(I2C_DEV->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED(pclk1, I2C_DEV_CLOCKSPEED, I2C_DUTYCYCLE_2));

       // I2Cx CR1 Configuration - Configure I2Cx: Generalcall and NoStretch mode
      MODIFY_REG(I2C_DEV->CR1, (I2C_CR1_ENGC | I2C_CR1_NOSTRETCH), (I2C_GENERALCALL_DISABLE| I2C_NOSTRETCH_DISABLE));

       // I2Cx OAR1 Configuration - Configure I2Cx: Own Address1 and addressing mode
      MODIFY_REG(I2C_DEV->OAR1, (I2C_OAR1_ADDMODE | I2C_OAR1_ADD8_9 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD0), I2C_ADDRESSINGMODE_7BIT);

       // I2Cx OAR2 Configuration - Configure I2Cx: Dual mode and Own Address2
      MODIFY_REG(I2C_DEV->OAR2, (I2C_OAR2_ENDUAL | I2C_OAR2_ADD2), I2C_DUALADDRESS_DISABLE);

      // Enable the selected I2C peripheral
    SET_BIT(I2C_DEV->CR1, I2C_CR1_PE);

    ssd1306_Init();	//Initialise SSD1306 OLED.
}

/**
 * @brief Task function for oled display.
 * 
 */
void s4595664TaskOled(void)
{
    portDISABLE_INTERRUPTS();
    s4595664_reg_oled_init();
    portENABLE_INTERRUPTS();
    
    s4595664SemaphoreUpdateOled = xSemaphoreCreateBinary();
    //s4595664SemaphoreStartSim = xSemaphoreCreateBinary();
    s4595664MutexOled = xSemaphoreCreateMutex();

    // if (s4595664SemaphoreStartSim != NULL)
    // {
    //     xSemaphoreGive(s4595664SemaphoreStartSim);
    // }

    // ssd1306_SetCursor(10,10);
    // ssd1306_WriteString("CSSE3010 OLED Test", Font_6x8, SSD1306_WHITE);
    // taskENTER_CRITICAL();
    // ssd1306_UpdateScreen();
    // taskEXIT_CRITICAL();

    // vTaskDelay(500 / portTICK_PERIOD_MS);

    for (;;)
    {
        if (s4595664SemaphoreUpdateOled != NULL) //semaphore exists
        {
            if (xSemaphoreTake(s4595664SemaphoreUpdateOled, 
                               10) == pdTRUE)
            // incoming msg notification
            {
                if (s4595664MutexOled != NULL) {

                    if (xSemaphoreTake(s4595664MutexOled, 
                        (TickType_t) 10) == pdTRUE) {

                        //portDISABLE_INTERRUPTS();
                        taskENTER_CRITICAL();
                        ssd1306_UpdateScreen();
                        taskEXIT_CRITICAL();
                        //portENABLE_INTERRUPTS();
                        xSemaphoreGive(s4595664MutexOled);
                    }
                }
            }
        }
        
        // if (s4595664SemaphoreStartSim != NULL)
        // {
        //     xSemaphoreGive(s4595664SemaphoreStartSim);
        // }

        vTaskDelay(10);
    }
}

/**
 * @brief Creates the task.
 * 
 */
void s4595664_tsk_oled_init(void)
{
    xTaskCreate((void *) &s4595664TaskOled, 
                (const signed char *) "OLED", 
                OLEDTASK_STACK_SIZE, 
                NULL, 
                OLEDTASK_PRIORITY, 
                NULL);
}

/**
 * @brief Clears the screen, then write value to oled screen.
 * 
 * @param msg 
 */
// void oled_write(oledTextMsg msg)
// {
//     portDISABLE_INTERRUPTS();
//     ssd1306_Fill(Black);
//     ssd1306_SetCursor(msg.startX,msg.startY);
//     ssd1306_WriteString(msg.displayText, Font_11x18, SSD1306_WHITE);
//     ssd1306_UpdateScreen();
//     portENABLE_INTERRUPTS();
// }

void s4595664_reg_oled_setCell(uint8_t x, uint8_t y)
{
    ssd1306_DrawPixel(2 * x, 2 * y, SSD1306_WHITE);
    ssd1306_DrawPixel(2 * x + 1, 2 * y, SSD1306_WHITE);
    ssd1306_DrawPixel(2 * x, 2 * y + 1, SSD1306_WHITE);
    ssd1306_DrawPixel(2 * x + 1, 2 * y + 1, SSD1306_WHITE);
}

void s4595664_reg_oled_clearCell(uint8_t x, uint8_t y)
{
    ssd1306_DrawPixel(2 * x, 2 * y, Black);
    ssd1306_DrawPixel(2 * x + 1, 2 * y, Black);
    ssd1306_DrawPixel(2 * x, 2 * y + 1, Black);
    ssd1306_DrawPixel(2 * x + 1, 2 * y + 1, Black);
}

void s4595664_reg_oled_clearMap(void)
{
    ssd1306_Fill(Black);
}
