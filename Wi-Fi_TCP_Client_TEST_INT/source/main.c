/* Header file includes. */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* FreeRTOS header file. */
#include <FreeRTOS.h>
#include <task.h>

/* TCP client task header file. */
#include "tcp_client.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* RTOS related macros. */
#define TCP_CLIENT_TASK_STACK_SIZE        (5 * 1024)
#define TCP_CLIENT_TASK_PRIORITY          (1)

/*******************************************************************************
* Global Variables
********************************************************************************/
/* This enables RTOS aware debugging. */
volatile int uxTopUsedPriority;

//the global look up table to map keyword

char *keywords[] = {"apple", "banana", "cherry", "date", "elderberry"};

int main()
{
    cy_rslt_t result;

    /* This enables RTOS aware debugging in OpenOCD. */
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the board support package. */
    result = cybsp_init() ;
    CY_ASSERT(result == CY_RSLT_SUCCESS);
    
    /* To avoid compiler warnings. */
    (void) result;

    /* Enable global interrupts. */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port. */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    
    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("============================================================\n");
    printf("CE229112 - Connectivity Example: TCP Client\n");
    printf("============================================================\n\n");

    /* Create the tasks. */
    printf("%s\n", keywords[2]);
    xTaskCreate(tcp_client_task, "Network task", TCP_CLIENT_TASK_STACK_SIZE, (void *)keywords[2], TCP_CLIENT_TASK_PRIORITY, NULL);

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never get here. */
    CY_ASSERT(0);
}
 
 /* [] END OF FILE */ 
