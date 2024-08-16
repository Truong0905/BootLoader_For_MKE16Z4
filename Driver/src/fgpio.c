/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stddef.h>
#include <stdbool.h>
#include "MKE16Z4.h"
#include "fgpio.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
FGPIO_Type *s_ArrayFGPIO[5] = FGPIO_BASE_PTRS;
/*******************************************************************************
 * Code
 ******************************************************************************/

void FGPIO_Init(const FGPIO_Config_struct_t *const pFGPIOConfig)
{
    uint8_t pinSelect = 0;
    FGPIO_Type *pFGPIOx = s_ArrayFGPIO[pFGPIOConfig->fgpioX];

    pinSelect = pFGPIOConfig->pin;

    if (true == pFGPIOConfig->outputMode)
    {
        pFGPIOx->PDDR |= 1 << pinSelect; /*Mode output*/
    }
    else
    {
        pFGPIOx->PDDR &= ~(1 << pinSelect); /* Input mode */
    }
}

void FGPIO_WriteToSetPin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin)
{
    s_ArrayFGPIO[fgpioX]->PSOR = 1 << pin;
}

void FGPIO_WriteToClearPin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin)
{

    s_ArrayFGPIO[fgpioX]->PCOR = 1 << pin;
}

void FGPIO_TogglePin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin)
{

    s_ArrayFGPIO[fgpioX]->PTOR = 1 << pin;
}

bool FGPIO_ReadPin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin)
{
    return (1 == ((s_ArrayFGPIO[fgpioX]->PDIR >> pin) & (1u)));
}
