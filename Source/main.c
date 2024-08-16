/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "boot.h"
/*******************************************************************************
 * Code
 ******************************************************************************/

/**
 * @brief  The entry point.
 *  @return uint8_t
 */
uint8_t main(void)
{
    volatile uint8_t exceptionReturn = 0; /*create a volatile variable to avoid warning that can't exit the while(1) loop*/
    bool status = true;

    BOOT_InitButton();

    if (false == BOOT_PressButton())
    {
        BOOT_APP_Task();
    }
    else
    {
        /*If the boot button is pressed*/
        status = BOOT_Init();
    }

    while (1)
    {
        if (true == status)
        {
            status = BOOT_Task();
        }

        if (0 != exceptionReturn)
        {
            break;
        }
    }

    return 0;
}
