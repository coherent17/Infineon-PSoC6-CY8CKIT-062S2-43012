#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define GPIO_INTERRUPT_PRIORITY (7u)

volatile bool pressFlag = false;

// UART object and configuration structure
cyhal_uart_t uart_obj;
const cyhal_uart_cfg_t uart_config =
{
	.data_bits = 8,
	.stop_bits = 1,
	.parity = CYHAL_UART_PARITY_NONE,
	.rx_buffer = NULL,
	.rx_buffer_size = 0
};

//Interrupt handler
static void button_isr(void *handler_arg, cyhal_gpio_event_t event)
{
	//set press flag and toggle led
	cyhal_gpio_toggle(CYBSP_USER_LED);
	pressFlag = true;
}

// GPIO callback initialization structure
cyhal_gpio_callback_data_t cb_data =
{
	.callback     = button_isr,
	.callback_arg = NULL
};

int main(void)
{
    cy_rslt_t result;
	uint8_t numberPresses = 0;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    /*BSP init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize the user button */
	result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize UART */
	result = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
	/* UART init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Configure GPIO interrupt */
	cyhal_gpio_register_callback(CYBSP_USER_BTN, &cb_data);
	cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);

	// enable interrupts
    __enable_irq();

    for (;;)
    {
    	if(pressFlag)
		{
    		numberPresses++;
    		if(numberPresses > 9)
			{
    			numberPresses = 0;
    		}

    		//create char from int to send over UART
    		char printChar = numberPresses + '0';
    		cyhal_uart_putc(&uart_obj, printChar);
    		cyhal_uart_putc(&uart_obj, '\n');
    		cyhal_uart_putc(&uart_obj, '\r');
    		pressFlag = false;
    	}
    }
}
/* [] END OF FILE */
