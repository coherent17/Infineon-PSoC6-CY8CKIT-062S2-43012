#include "cy_pdl.h"
#include "cyhal.h"
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

    cyhal_pwm_t MyHAL_PWM;
    cyhal_pwm_init_adv(&MyHAL_PWM, CYBSP_USER_LED, NC, CYHAL_PWM_LEFT_ALIGN, true, 0, false, NULL);
    cyhal_pwm_set_duty_cycle(&MyHAL_PWM, 50, 1);
    cyhal_pwm_start(&MyHAL_PWM);


    Cy_TCPWM_PWM_Init(MyPDL_PWM_HW, MyPDL_PWM_NUM, &MyPDL_PWM_config);
    Cy_TCPWM_PWM_Enable(MyPDL_PWM_HW, MyPDL_PWM_NUM);
    Cy_TCPWM_TriggerReloadOrIndex_Single(MyPDL_PWM_HW, MyPDL_PWM_NUM);

    for (;;)
    {
    	cyhal_syspm_sleep();
    }
}
