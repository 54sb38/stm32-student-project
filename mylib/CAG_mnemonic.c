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
 * s4595664_CAG_mnemonic_init() - Init task
 *************************************************************** 
 * EXTERNAL GLOBAL VARIABLES 
 ***************************************************************
 ***************************************************************
 */

#include "CAG_mnemonic.h"

void cliTask(void);
void s4595664_CAG_mnemonic_init(void);
void hardware_init();

static BaseType_t prvEchoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString );
static BaseType_t prvStillCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString );
static BaseType_t prvStartCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );
static BaseType_t prvStopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );
static BaseType_t prvOscCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString );
static BaseType_t prvGliderCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString );
static BaseType_t prvDelCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );
static BaseType_t prvCreCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );
static BaseType_t prvSystemCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );
static BaseType_t prvUsageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString );

CLI_Command_Definition_t xEcho = {	// Structure that defines the "echo" command line command.
	"echo",							// Comamnd String
	"echo: Echo the input.\r\n",	// Help String (Displayed when "help' is typed)
	prvEchoCommand,					// Command Callback that implements the command
	1								// Number of input parameters
};

CLI_Command_Definition_t xStill = {	// Structure that defines the "still" command line command.
	"still",							// Comamnd String
	"still: spawn a still obj.\r\n",	// Help String (Displayed when "help' is typed)
	prvStillCommand,					// Command Callback that implements the command
	3								// Number of input parameters
};

CLI_Command_Definition_t xOsc = {	// Structure that defines the "osc" command line command.
	"osc",							// Comamnd String
	"osc: spawn a osc obj.\r\n",	// Help String (Displayed when "help' is typed)
	prvOscCommand,					// Command Callback that implements the command
	3								// Number of input parameters
};

CLI_Command_Definition_t xGlider = {	// Structure that defines the "glider" command line command.
	"glider",							// Comamnd String
	"still: spawn a still obj.\r\n",	// Help String (Displayed when "help' is typed)
	prvGliderCommand,					// Command Callback that implements the command
	2								// Number of input parameters
};

CLI_Command_Definition_t xStart = {	// Structure that defines the "start" command line command.
	"start",							// Comamnd String
	"start: starts simulation.\r\n",	// Help String (Displayed when "help' is typed)
	prvStartCommand,					// Command Callback that implements the command
	0								// Number of input parameters
};

CLI_Command_Definition_t xStop = {	// Structure that defines the "stop" command line command.
	"stop",							// Comamnd String
	"stop: stops simulation.\r\n",	// Help String (Displayed when "help' is typed)
	prvStopCommand,					// Command Callback that implements the command
	0								// Number of input parameters
};

CLI_Command_Definition_t xClear = {	// Structure that defines the "clear" command line command.
	"clear",							// Comamnd String
	"clear: clear display and reset.\r\n",	// Help String (Displayed when "help' is typed)
	prvClearCommand,					// Command Callback that implements the command
	0								// Number of input parameters
};

CLI_Command_Definition_t xDel = {	// Structure that defines the "start" command line command.
	"del",							// Comamnd String
	"del: deletes simulation.\r\n",	// Help String (Displayed when "help' is typed)
	prvDelCommand,					// Command Callback that implements the command
	1								// Number of input parameters
};

CLI_Command_Definition_t xCre = {	// Structure that defines the "start" command line command.
	"cre",							// Comamnd String
	"cre: creates simulation.\r\n",	// Help String (Displayed when "help' is typed)
	prvCreCommand,					// Command Callback that implements the command
	1								// Number of input parameters
};

CLI_Command_Definition_t xSystem = {	// Structure that defines the "start" command line command.
	"system",							// Comamnd String
	"system: current sys time.\r\n",	// Help String (Displayed when "help' is typed)
	prvSystemCommand,					// Command Callback that implements the command
	0								// Number of input parameters
};

CLI_Command_Definition_t xUsage = {	// Structure that defines the "start" command line command.
	"usage",							// Comamnd String
	"usage: show task list.\r\n",	// Help String (Displayed when "help' is typed)
	prvUsageCommand,					// Command Callback that implements the command
	0								// Number of input parameters
};

// Task Priorities (Idle Priority is the lowest priority)
#define CLI_PRIORITY					( tskIDLE_PRIORITY + 5 )

// Task Stack Allocations (must be a multiple of the minimal stack size)
#define CLI_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 4 )

#define SIMTASK 0
#define JOYTASK 1

/*
 * Initialise CLI
 */
void s4595664_CAG_mnemonic_init(void) {

	/* Register CLI commands */
	FreeRTOS_CLIRegisterCommand(&xEcho);
    FreeRTOS_CLIRegisterCommand(&xStill);
    FreeRTOS_CLIRegisterCommand(&xOsc);
    FreeRTOS_CLIRegisterCommand(&xGlider);
    FreeRTOS_CLIRegisterCommand(&xStart);
    FreeRTOS_CLIRegisterCommand(&xStop);
    FreeRTOS_CLIRegisterCommand(&xClear);
    FreeRTOS_CLIRegisterCommand(&xDel);
    FreeRTOS_CLIRegisterCommand(&xCre);
    FreeRTOS_CLIRegisterCommand(&xSystem);
    FreeRTOS_CLIRegisterCommand(&xUsage);

	/* Create CLI Receiving task */
	xTaskCreate((void *) &cliTask, (const signed char *) "CLI", 
                CLI_TASK_STACK_SIZE, NULL, CLI_PRIORITY, NULL );

}

/*
 * CLI Receiving Task.
 */
void cliTask(void) {

	int i;
	char cRxedChar;
	char cInputString[100];
	int InputIndex = 0;
	char *pcOutputString;
	BaseType_t xReturned;
    bool useMne = false;

	hardware_init();

	/* Initialise pointer to CLI output buffer. */
	memset(cInputString, 0, sizeof(cInputString));
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	for (;;) {

        if (inputModeEventGroup != NULL)
        {
            if (!!xEventGroupGetBits(inputModeEventGroup))
            {
                // toggle grid use
                useMne = true;
                BRD_LEDGreenOff();
                //while (BRD_debuguart_getc() != '\0'); // clear rx buffer

            } else {

                useMne = false;
            }
        }

        if (useMne) {
            
            /* Receive character from terminal */
            cRxedChar = debug_getc();

            /* Process if character if not Null */
            if (cRxedChar != '\0') {

                //BRD_LEDGreenToggle();		//Toggle Green LED when a byte is character.

                /* Echo character */

                debug_putc(cRxedChar);

                /* Process only if return is received. */
                if (cRxedChar == '\r') {

                    //Put new line and transmit buffer

                    debug_putc('\n');
                    debug_flush();

                    /* Put null character in command input string. */
                    cInputString[InputIndex] = '\0';

                    xReturned = pdTRUE;
                    /* Process command input string. */
                    while (xReturned != pdFALSE) {

                        /* Returns pdFALSE, when all strings have been returned */
                        xReturned = FreeRTOS_CLIProcessCommand( cInputString, 
                            pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

                        /* Display CLI command output string (not thread safe) */

                        portENTER_CRITICAL();
                        for (i = 0; i < (int) strlen(pcOutputString); i++) {
                            debug_putc(*(pcOutputString + i));
                            
                        }
                        portEXIT_CRITICAL();

                        vTaskDelay(10);	//Must delay between debug_printfs.
                    }
                    memset(pcOutputString, 0, configCOMMAND_INT_MAX_OUTPUT_SIZE);

                    memset(cInputString, 0, sizeof(cInputString));
                    InputIndex = 0;

                } else {

                    debug_flush();		//Transmit USB buffer

                    if( cRxedChar == '\r' ) {

                        /* Ignore the character. */
                    } else if( cRxedChar == '\b' ) {

                        /* Backspace was pressed.  Erase the last character in the
                        string - if any.*/
                        if( InputIndex > 0 ) {
                            InputIndex--;
                            cInputString[ InputIndex ] = '\0';
                        }

                    } else {

                        /* A character was entered.  Add it to the string
                        entered so far.  When a \n is entered the complete
                        string will be passed to the command interpreter. */
                        if( InputIndex < 20 ) {
                            cInputString[ InputIndex ] = cRxedChar;
                            InputIndex++;
                        }
                    }
                }
            }
        }
		vTaskDelay(50);
		
	}
}

void hardware_init( void ) {

	portDISABLE_INTERRUPTS();	//Disable interrupts

	BRD_LEDInit();				//Initialise Green LED
	BRD_LEDGreenOff();			//Turn off Green LED
	BRD_debuguart_init();  		//Initialise UART for debug log output

	portENABLE_INTERRUPTS();	//Enable interrupts
}

/*
 * Echo Command.
 */
static BaseType_t prvEchoCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString ) {

	long lParam_len;
	const char *cCmd_string;

	/* Get parameters from command string */
	cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

	/* Write command echo output string to write buffer. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\n\r%s\n\r", cCmd_string);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Still Command.
 */
static BaseType_t prvStillCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString ) {

	long lParam_len;
	const char *type, *x, *y;

	/* Get parameters from command string */
	type = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    x = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);
    y = FreeRTOS_CLIGetParameter(pcCommandString, 3, &lParam_len);

    caMessage_t txMsg;
    txMsg.cell_x = (int) atoi(x);
    txMsg.cell_y = (int) atoi(y);
    txMsg.type = (int) atoi(type) | (STILL << 4);

    xQueueSend(s4595664CaMessageQueue, &txMsg, 10);
    xSemaphoreGive(s4595664SpawnPattern);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * osc Command.
 */
static BaseType_t prvOscCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString ) {

	long lParam_len;
	const char *type, *x, *y;

	/* Get parameters from command string */
	type = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    x = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);
    y = FreeRTOS_CLIGetParameter(pcCommandString, 3, &lParam_len);

    caMessage_t txMsg;
    txMsg.cell_x = (int) atoi(x);
    txMsg.cell_y = (int) atoi(y);
    txMsg.type = (int) atoi(type) | (OSC << 4);

    xQueueSend(s4595664CaMessageQueue, &txMsg, 10);
    xSemaphoreGive(s4595664SpawnPattern);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Glider Command.
 */
static BaseType_t prvGliderCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                 const char *pcCommandString ) {

	long lParam_len;
	const char *type, *x, *y;

	/* Get parameters from command string */
	//type = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    x = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    y = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);

    caMessage_t txMsg;
    txMsg.cell_x = (int) atoi(x);
    txMsg.cell_y = (int) atoi(y);
    txMsg.type = 0 | (GLIDER << 4);

    xQueueSend(s4595664CaMessageQueue, &txMsg, 10);
    xSemaphoreGive(s4595664SpawnPattern);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Start Command.
 */
static BaseType_t prvStartCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{

    ( void ) xWriteBufferLen;

    xEventGroupSetBits(simConfigsEventGroup, ONOFFEVENTBIT);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Stop Command.
 */
static BaseType_t prvStopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{

    ( void ) xWriteBufferLen;

    xEventGroupClearBits(simConfigsEventGroup, ONOFFEVENTBIT);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Clear Command.
 */
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{

    ( void ) xWriteBufferLen;

    xEventGroupSetBits(simConfigsEventGroup, CLEARGRIDBIT);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/**
 * @brief del command
 * 
 * @param pcWriteBuffer 
 * @param xWriteBufferLen 
 * @param pcCommandString 
 * @return BaseType_t 
 */
static BaseType_t prvDelCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{

	long lParam_len;
	const char *type;

	/* Get parameters from command string */
	type = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    int task2delete = atoi(type);

    switch (task2delete)
    {
        case (SIMTASK):

            s4595664_tsk_CAG_simulator_deInit();
            break;

        case (JOYTASK):

            s4595664_tsk_CAG_joystick_deInit();
            break;
    }

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/**
 * @brief create task cmd
 * 
 * @param pcWriteBuffer 
 * @param xWriteBufferLen 
 * @param pcCommandString 
 * @return BaseType_t 
 */
static BaseType_t prvCreCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{

	long lParam_len;
	const char *type;

	/* Get parameters from command string */
	type = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);
    int task2cre = atoi(type);

    switch (task2cre)
    {
        case (SIMTASK):

            s4595664_tsk_CAG_simulator_init();
            break;

        case (JOYTASK):

            s4595664_tsk_CAG_joystick_init();
            break;
    }

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

static BaseType_t prvSystemCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{

    ( void ) xWriteBufferLen;

    TickType_t sysTime = xTaskGetTickCount();
    debug_log("System time: %d\n\r", sysTime / portTICK_PERIOD_MS);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

static BaseType_t prvUsageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
                                  const char *pcCommandString ) 
{
    (void) xWriteBufferLen;
    (void *) pcCommandString;
    
    vTaskList(pcWriteBuffer);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}
