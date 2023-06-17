#include "cyhal.h"    //Hardware Abstraction Layer
#include "cy_pdl.h"   //Peripheral Driver Library
#include "cybsp.h"

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    //init the gpio
    cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_ON);


    for (;;)
    {
    	cyhal_gpio_toggle(CYBSP_LED_RGB_BLUE);
    	Cy_GPIO_Inv(CYBSP_LED_RGB_GREEN_PORT, CYBSP_LED_RGB_GREEN_PIN);
    	//cyhal_system_delay_ms(500);
    	Cy_SysLib_Delay(500);
    }
}
