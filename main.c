/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSOC™ Control C1 MCU: RTC Alarm Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*****************************************************************************/
#include "cybsp.h"
#include "cy_utils.h"
#include "cy_retarget_io.h"
#include <stdio.h>

/*******************************************************************************
* Macros
*******************************************************************************/

/*SysTick timer frequency in Hz*/
#define TICKS_PER_SECOND                1000

/*USER LED toggle period in milliseconds*/
#define TICKS_WAIT                      500


#define RTC_INTERRUPT_EVENT_IRQN        SCU_1_IRQn
#define RTC_INTERRUPT_EVENT_PRIORITY    3
#define alarm_handler                   SCU_1_IRQHandler



/*******************************************************************************
* Defines
*******************************************************************************/

/* Define macro to enable/disable printing of debug messages */
#define ENABLE_DEBUG_PRINT              (0)

/*******************************************************************************
* Global Variables
*******************************************************************************/

/*Variable for keeping track of time*/
static volatile uint32_t ticks = 0;

/*Variable for keeping track of Interrupt*/
static volatile bool timer_interrupt_flag = false;

/*******************************************************************************
* Function Name: SysTick_Handler
********************************************************************************
* Summary:
* This is System tick interrupt service routine
*
* Parameters:
*  none
*
* Return:
*  none
*
*******************************************************************************/

void SysTick_Handler(void)
{
    ticks++;
    if (TICKS_WAIT == ticks)
    {
        Cy_GPIO_ToggleOutput(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        ticks = 0;
    }
}

/*******************************************************************************
* Function Name: alarm_handler
********************************************************************************
* Summary:
* This is RTC alarm interrupt service routine
*
* Parameters:
*  none
*
* Return:
*  none
*
*******************************************************************************/
void alarm_handler(void)
{
    /*Stop SysTick timer*/
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    timer_interrupt_flag = true;
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function. The RTC demo example programs the RTC Alarm for one minute.
* System Tick Interrupt continues blinking the LED on Eval Kit at approximately 1Hz.
* RTC alarm interrupt is triggered after 1 minute.
* It will stop blinking the LED on Eval Kit.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/

int main(void)
{
    cy_rslt_t result;
    #if ENABLE_DEBUG_PRINT
    /* Assign false to disable printing of debug messages */
    static volatile bool debug_printf = true;
    #endif

    /*Initialize the device and board peripherals*/
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize printf retarget */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_HW);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    #if ENABLE_DEBUG_PRINT
    printf("Initialization done\r\n");
    #endif

    /*Set the Priority for Interrupt*/
    NVIC_SetPriority(RTC_INTERRUPT_EVENT_IRQN, RTC_INTERRUPT_EVENT_PRIORITY);


    /*Enable the SCU Interrupt Event*/
    Cy_SCU_INTERRUPT_EnableEvent(CY_SCU_INTERRUPT_EVENT_RTC_ALARM);

    /*Enable the Interrupt*/
    NVIC_EnableIRQ(RTC_INTERRUPT_EVENT_IRQN);

    /*System timer configuration*/
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

    /*Infinite loop*/
    while (1)
    {
        #if ENABLE_DEBUG_PRINT
        if(debug_printf)
        {
            /* Print message after the SysTick_Handler is triggered */
            printf("System Tick Interrupt blinks the LED for one minute\r\n");
            debug_printf = false;
        }
        #endif

        /*Check if timer elapsed (interrupt fired) and toggle the LED*/
        if (timer_interrupt_flag)
        {
            /*Clear the flag*/
            timer_interrupt_flag = false;

            /*Set to User LED state*/
            Cy_GPIO_SetOutputLow(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);

            #if ENABLE_DEBUG_PRINT
            /* Print message after the RTC alarm interrupt is triggered */
            printf("The RTC alarm interrupt is triggered, LED stop blinking\r\n");
            #endif
        }
    }
}

/* [] END OF FILE */
