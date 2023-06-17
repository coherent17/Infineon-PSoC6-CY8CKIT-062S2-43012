#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

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

int main(void)
{
    cy_rslt_t result;
    uint8_t read_data;

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

	/* Initialize UART */
	result = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
	/* UART init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	// enable interrupts
    __enable_irq();

    for (;;)
     {
    	cyhal_uart_getc(&uart_obj, &read_data, 0);
		if(read_data == '0')
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
		}
		else if(read_data == '1')
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
		}
     }
 }
