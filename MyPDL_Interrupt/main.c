#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

bool g_event_occurred = false;

static void MyGPIO_ISR(void){
	Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_NUM);
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

    cy_stc_sysint_t irqcfg = {CYBSP_USER_BTN_IRQ, 7};
    cy_SysInt_Init(&irqcfg, MyGPIO_ISR);
    INVIC_EnableIRQ(CYBSP_USER_IRQ);

    for (;;)
    {
    	if(g_event_occurred){
    		Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_NUM);
    		g_event_occurred = false;
    	}
    	Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}
