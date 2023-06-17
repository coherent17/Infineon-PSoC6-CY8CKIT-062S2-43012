#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"


//global variable as the call back function flag
bool g_event_occurred = false;

//the call back function definition
void MyCallbackFunction(void *callback_arg, cyhal_gpio_event_t event){
	g_event_occurred = true;
}

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

    //init the button and the led
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    //the data structure for the call back function
    cyhal_gpio_callback_data_t my_callback_data = {.callback = MyCallbackFunction};

    //set the call back function onto the specific pin (button here)
    cyhal_gpio_register_callback(CYBSP_USER_BTN, &my_callback_data);

    //when event is trigger, then execute the call back function
    //the signal is pulled high, when the user push the button, the signal will be low -> falling
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, CYHAL_ISR_PRIORITY_DEFAULT, true);


    for (;;)
    {
    	if(g_event_occurred){
    		cyhal_gpio_toggle(CYBSP_USER_LED);
    		g_event_occurred = false;
    	}
    	//place the device in low power mode, and wake up a system event if interrupted
    	cyhal_syspm_deepsleep();
    }
}
