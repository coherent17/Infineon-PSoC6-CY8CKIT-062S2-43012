#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

volatile int uxTopUsedPriority;

static SemaphoreHandle_t ButtonSemaphore;

/* Button ISR */
void button_interrupt_handler(void* handler_arg, cyhal_gpio_event_t event)
{
    static BaseType_t xHigherPriorityTaskWoken;

    /* Give the semaphore */
    xSemaphoreGiveFromISR(ButtonSemaphore, &xHigherPriorityTaskWoken);

    /* Yield current task if a higher priority task is now unblocked */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Structure for GPIO interrupt */
cyhal_gpio_callback_data_t button_interrupt_data =
{
    .callback     = button_interrupt_handler,
    .callback_arg = NULL
};

/* Task to handle the LED */
void led_task()
{
    while (1)
    {
        /* Wait for button press */
    	xSemaphoreTake(ButtonSemaphore, portMAX_DELAY);

    	/* Toggle LED */
        cyhal_gpio_toggle(CYBSP_USER_LED);
    }
}

int main(void)
{
    cy_rslt_t result;

    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* Initialize button with an interrupt of priority 3 */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);
	cyhal_gpio_register_callback(CYBSP_USER_BTN, &button_interrupt_data);
	cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, 3, true);

	/* Create Semaphore and take it so that it doesn't trigger an initial toggle of the LED */
     vSemaphoreCreateBinary(ButtonSemaphore);
     xSemaphoreTake(ButtonSemaphore, 0);

    /* Create task with a stack of 1024, no parameters, and a priority of 1 */
    xTaskCreate(led_task, (char *)"led_task", 1024, 0, 1, NULL);

    /* Start the RTOS scheduler */
    vTaskStartScheduler();
    CY_ASSERT(0);
}
