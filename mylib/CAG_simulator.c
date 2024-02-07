 /** 
 **************************************************************
 * @file mylib/CAG_mnemonic.c
 * @author Jingbo Ma - 45956649
 * @date 21052022
 * @brief CAG joystick driver
 * REFERENCE: csse3010_project.pdf 
 *            stm32f429zi_reference.pdf
 *            stm32f429zi_datasheet.pdf
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4595664_tsk_CAG_simulator_init();
 * s4595664_CAG_clear_grid();
 * s4595664_CAG_grid_spawn();
 * s4595664_CAG_grid_kill();
 * s4595664_tsk_CAG_simulator_deInit();
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 ***************************************************************
 */

#include "CAG_simulator.h"

Cell* map[GRID_WIDTH];
Cell* mapBuffer[GRID_WIDTH];

EventGroupHandle_t simConfigsEventGroup;
SemaphoreHandle_t s4595664MutexGrid;
SemaphoreHandle_t s4595664SpawnPattern;
QueueHandle_t s4595664CaMessageQueue;
TaskHandle_t xSimulatorHandle;

void CAG_simulator_init(void);
void CAG_processing(void);
void CAG_simulator(void);
void syncMapBuffer(void);
void getNeighboursInfo(Cell** mapName, int8_t x, int8_t y, int8_t* buffer);
Cell* getCell(Cell** mapName, int8_t x, int8_t y);
void spawn(Cell** mapName, int8_t x, int8_t y, uint32_t stateValue);
void kill(Cell** mapName, int8_t x, int8_t y);
void incrementStateValue(Cell** mapName, int8_t x, int8_t y);
void send2Display(void);
void s4595664_tsk_CAG_simulator_init(void);
void s4595664_CAG_clear_grid(void);
void s4595664_CAG_grid_spawn(int8_t x, int8_t y);
void s4595664_CAG_grid_kill(int8_t x, int8_t y);
void s4595664_spawn_pattern(int8_t x, int8_t y, int pattern, int type);
void spawn_still(int8_t x, int8_t y, int type);
void spawn_osc(int8_t x, int8_t y, int type);
void spawn_glider(int8_t x, int8_t y);


void put_some_cells(void);


void s4595664_tsk_CAG_simulator_init(void)
{
    xTaskCreate((void *) &CAG_simulator, 
                (const signed char *) "SIM", 
                CAGSIMTASK_STACK_SIZE, 
                NULL, 
                CAGSIMTASK_PRIORITY, 
                &xSimulatorHandle);
}

void s4595664_tsk_CAG_simulator_deInit(void)
{
    for (int x = 0; x < GRID_WIDTH; ++x)
    {
        vPortFree(map[x]);
        vPortFree(mapBuffer[x]);
    }
    vTaskDelete(xSimulatorHandle);
}

void CAG_simulator_init(void)
{
    for (int x = 0; x < GRID_WIDTH; ++x) {

        map[x] = pvPortMalloc(sizeof(Cell) * GRID_HEIGHT);
        mapBuffer[x] = pvPortMalloc(sizeof(Cell) * GRID_HEIGHT);

        for (int y = 0; y < GRID_HEIGHT; ++y) {

            Cell cell;
            cell.alive = false;
            cell.stateValue = 0;
            map[x][y] = cell;
            // map[x][y].alive = false;
            // map[x][y].stateValue = 0;
            mapBuffer[x][y] = cell;
            // mapBuffer[x][y].alive = false;
            // mapBuffer[x][y].stateValue = 0;
        }
    }

    //put_some_cells();
}

void CAG_simulator(void)
{
    BRD_LEDInit();
    simConfigsEventGroup = xEventGroupCreate();
    s4595664MutexGrid = xSemaphoreCreateMutex();
    s4595664SpawnPattern = xSemaphoreCreateBinary();
    s4595664CaMessageQueue = xQueueCreate(1, sizeof(caMessage_t));
    
    CAG_simulator_init();


    EventBits_t uxBits = 0;
    bool on = true;
    uint16_t sleepTime = 2000 / portTICK_PERIOD_MS;

    for (;;)
    {
        if (simConfigsEventGroup != NULL)
        {
            uxBits = xEventGroupWaitBits(simConfigsEventGroup, CLEARGRIDBIT, 
                                         pdTRUE, pdFALSE, 10);

            on = (bool) (uxBits & ONOFFEVENTBIT);
            if(!!(uxBits & CLEARGRIDBIT))
            {
                s4595664_CAG_clear_grid();
            }

            int sleepTimeBits = (uxBits & SLEEPTIMEBITS) >> 1;
            switch (sleepTimeBits) {

                case 0:

                    sleepTime = 1000 / portTICK_PERIOD_MS;
                    break;
                
                case 0b01:

                    sleepTime = 2000 / portTICK_PERIOD_MS;
                    break;

                case 0b10:

                    sleepTime = 5000 / portTICK_PERIOD_MS;
                    break;

                case 0b11:

                    sleepTime = 10000 / portTICK_PERIOD_MS;
                    break;
                    
            }
        }

        if (s4595664SpawnPattern != NULL)
        {
            if (xSemaphoreTake(s4595664SpawnPattern, 
                (TickType_t) 10) == pdTRUE) {

                caMessage_t rxMsg;

                if( xQueueReceive(s4595664CaMessageQueue,
                    &( rxMsg ),
                    ( TickType_t ) 10 ) == pdPASS) {

                    //debug_log("change pattern\n\r");
                    int pattern = rxMsg.type >> 4;
                    int type = rxMsg.type & 0b1111;
                    s4595664_spawn_pattern(rxMsg.cell_x,
                                           rxMsg.cell_y,
                                           pattern, type);
                }
            }
        }

        if (on) {

            BRD_LEDBlueOff();

            // if (s4595664SemaphoreUpdateOled != NULL) {

            //     xSemaphoreGive(s4595664SemaphoreUpdateOled);
            // }

            CAG_processing();
            // send2Display();

        } else {

            BRD_LEDBlueOn();
        }

        // update screendisplay
        send2Display();

        vTaskDelay(sleepTime);
    }
}

void CAG_processing(void)
{
    for (int x = 0; x < GRID_WIDTH; ++x) {

        for (int y = 0; y < GRID_HEIGHT; ++y) {

            Cell* cell = getCell(mapBuffer, x, y);
            uint8_t neighboursInfo[2] = {0, 0};
            getNeighboursInfo(mapBuffer, x, y, neighboursInfo);
            uint8_t liveNeighboursCount = neighboursInfo[0];
            uint8_t maxStateValue = neighboursInfo[1];

            if (cell != NULL) {

                if (cell->alive) { // live cell

                    if ((liveNeighboursCount == 2) || 
                        (liveNeighboursCount == 3))
                    {
                        incrementStateValue(map, x, y);

                    } else {

                        kill(map, x, y);
                    }

                } else {// dead cell 

                    if (liveNeighboursCount == 3) {

                        spawn(map, x, y, maxStateValue);
                    }
                }
            }
        }
    }
    
    syncMapBuffer();
}

void syncMapBuffer(void)
{
    for (int x = 0; x < GRID_WIDTH; ++x) { // update buffer map

        memcpy(mapBuffer[x], map[x], sizeof(Cell) * GRID_HEIGHT);
    }
}

void getNeighboursInfo(Cell** mapName, int8_t x, int8_t y, int8_t* buffer)
{
    uint8_t count = 0;
    uint8_t maxStateValue = 0;
    Cell* cell = getCell(mapName, x, y);

    for (int i = -1; i < 2; ++i) {

        for (int j = -1; j < 2; ++j) {

            if ((i == 0) && (j == 0)) {

                continue;
            }

            Cell* temp = getCell(mapName, x + i, y + j);
            if (temp != NULL) {

                if (temp->alive) {

                    ++count;
                    if (temp->stateValue > maxStateValue)
                    {
                        maxStateValue = temp->stateValue;
                    }
                }
            }
        }
    }

    buffer[0] = count;
    buffer[1] = maxStateValue;
}

Cell* getCell(Cell** mapName, int8_t x, int8_t y)
{
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {

        return NULL;
    }

    return &mapName[x][y];
}

void spawn(Cell** mapName, int8_t x, int8_t y, uint32_t stateValue)
{
    if (s4595664MutexGrid != NULL) {

        if (xSemaphoreTake(s4595664MutexGrid, 
            (TickType_t) 10) == pdTRUE) {

            Cell* cell = getCell(mapName, x, y);
            if (cell != NULL)
            {
                cell->alive = true;
                cell->stateValue = stateValue;
            }

            xSemaphoreGive(s4595664MutexGrid);
        }
    }
}

void kill(Cell** mapName, int8_t x, int8_t y)
{
    if (s4595664MutexGrid != NULL) {

        if (xSemaphoreTake(s4595664MutexGrid, 
            (TickType_t) 10) == pdTRUE) {

            Cell* cell = getCell(mapName, x, y);
            cell->alive = false;
            cell->stateValue = 0;

            xSemaphoreGive(s4595664MutexGrid);
        }
    }
}

void incrementStateValue(Cell** mapName, int8_t x, int8_t y)
{
    Cell* cell = getCell(mapName, x, y);
    cell->stateValue++;
}

// debug
void put_some_cells(void)
{
    // one point at origin
    //spawn(map, 0, 0, 1);
    //s4595664_spawn_pattern(0, 0, STILL, BLOCK);
    s4595664_spawn_pattern(8, 4, OSC, BLINKER);
    s4595664_spawn_pattern(16, 4, OSC, BEACON);
    s4595664_spawn_pattern(24, 8, OSC, TOAD);
    s4595664_spawn_pattern(30, 10, GLIDER, 0);
}

void send2Display(void)
{
    if (s4595664MutexOled != NULL) {

        if (xSemaphoreTake(s4595664MutexOled, 
            (TickType_t) 10) == pdTRUE) {

            s4595664_reg_oled_clearMap();

            for (int y = 0; y < GRID_HEIGHT; ++y) {

                for (int x = 0; x < GRID_WIDTH; ++x) {

                    Cell* cell = getCell(mapBuffer, x, y);
                    if (cell->alive) {
                    
                        s4595664_reg_oled_setCell(x, y);

                    } 
                }   
            }
            xSemaphoreGive(s4595664MutexOled);
        }
    }

    if (s4595664SemaphoreUpdateOled != NULL) {

        xSemaphoreGive(s4595664SemaphoreUpdateOled);
    }
}

void s4595664_CAG_clear_grid(void)
{
    for (int x = 0; x < GRID_WIDTH; ++x)
    {
        for (int y = 0; y < GRID_HEIGHT; ++y)
        {
            kill(map, x, y);
        }
    }

    syncMapBuffer();
}

void s4595664_CAG_grid_spawn(int8_t x, int8_t y)
{
    Cell* cell = getCell(map, x, y);
    if (!(cell->alive))
    {
        spawn(map, x, y, 1);
        spawn(mapBuffer, x, y, 1);
    }
}

void s4595664_CAG_grid_kill(int8_t x, int8_t y)
{
    Cell* cell = getCell(map, x, y);
    if (cell->alive)
    {
        kill(map, x, y);
        kill(mapBuffer, x, y);
    }
}

void s4595664_spawn_pattern(int8_t x, int8_t y, int pattern, int type)
{
    switch (pattern)
    {
    case STILL:
        /* code */
        spawn_still(x, y, type);
        break;

    case OSC:

        spawn_osc(x, y, type);
        break;

    case GLIDER:

        spawn_glider(x, y);
        break;

    case CELL:

        s4595664_CAG_grid_spawn(x, y);
        break;
    }
}

void spawn_still(int8_t x, int8_t y, int type)
{
    switch (type)
    {
        case BLOCK:

            spawn(map, x, y, 1);
            spawn(map, x + 1, y, 1);
            spawn(map, x, y + 1, 1);
            spawn(map, x + 1, y + 1, 1);

            break;
        
        case BEEHIVE:

            spawn(map, x, y+1, 1);
            spawn(map, x+1, y, 1);
            spawn(map, x+2, y, 1);
            spawn(map, x+1, y+2, 1);
            spawn(map, x+2, y+2, 1);
            spawn(map, x+3, y+1, 1);

            break;

        case LOAF:

            spawn(map, x, y+1, 1);
            spawn(map, x+1, y, 1);
            spawn(map, x+2, y, 1);
            spawn(map, x+3, y+1, 1);
            spawn(map, x+3, y+2, 1);
            spawn(map, x+1, y+2, 1);
            spawn(map, x+2, y+3, 1);
            break;
    }
    syncMapBuffer();
}

void spawn_osc(int8_t x, int8_t y, int type)
{
    switch (type)
    {
        case BLINKER:

            spawn(map, x, y, 1);
            spawn(map, x+1, y, 1);
            spawn(map, x+2, y, 1);
            break;

        case TOAD:

            spawn(map, x, y, 1);
            spawn(map, x, y+1, 1);
            spawn(map, x, y+2, 1);
            spawn(map, x+1, y+1, 1);
            spawn(map, x+1, y+2, 1);
            spawn(map, x+1, y+3, 1);
            break;

        case BEACON:

            spawn(map, x, y, 1);
            spawn(map, x + 1, y, 1);
            spawn(map, x, y + 1, 1);
            spawn(map, x + 1, y + 1, 1);

            spawn(map, x+2, y+2, 1);
            spawn(map, x+2 + 1, y+2, 1);
            spawn(map, x+2, y + 1+2, 1);
            spawn(map, x+2 + 1, y + 1+2, 1);
            break;
    }
    syncMapBuffer();
}

void spawn_glider(int8_t x, int8_t y)
{
    spawn(map, x, y, 1);
    spawn(map, x+1, y, 1);
    spawn(map, x+2, y, 1);
    spawn(map, x+2, y+1, 1);
    spawn(map, x+1, y+2, 1);
    syncMapBuffer();
}
