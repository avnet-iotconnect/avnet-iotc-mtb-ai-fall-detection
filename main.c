#include <stdbool.h>
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"

#include "app.h"



int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();
    /* Initialize retarget-io to use the debug UART port */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    /* Initialize the User LED */

    /* Workaround for the inverse LED */
    #ifdef APP_LEDS_INVERSE
    #define INITIAL_STATE CYBSP_LED_STATE_ON
    #else
    #define INITIAL_STATE CYBSP_LED_STATE_OFF
    #endif

    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, INITIAL_STATE);

    CY_ASSERT(CY_RSLT_SUCCESS == result);

    printf("\x1b[2J\x1b[;H");

    /* Create the MQTT Client task. */
    xTaskCreate(app_task, "App Task", 8 * 1204, NULL, 2, NULL);
    
    
    // Create the fall task
    xTaskCreate(fed_task, "Fall Task", 2048, NULL, 3, NULL);


    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never get here. */
    CY_ASSERT(0);
}
