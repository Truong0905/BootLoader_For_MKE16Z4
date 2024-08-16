/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include "MKE16Z4.h"
#include "clocking.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PCC_SOURCE_SELECT_FIRC (3u) /*Used to select FIRC source when config peripheral clock */

/*******************************************************************************
 * Code
 ******************************************************************************/

void CLOCKING_FIRCDiv2Config(const CLOCKING_SGC_FircDiv2_t div2Value)
{
    /*Changes to FIRCDIV should be done when FAST IRC is disabled to
    prevent glitches tooutput divided clock.*/

    /*Fast IRC is disabled*/
    SCG->FIRCCSR &= ~SCG_FIRCCSR_FIRCEN_MASK;

    /*Clock configuration*/
    SCG->FIRCDIV = (SCG->FIRCDIV & (~SCG_FIRCDIV_FIRCDIV2_MASK)) | SCG_FIRCDIV_FIRCDIV2(div2Value);

    /*Fast IRC is enabled*/
    SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;
}

void CLOCKING_ConfigOnlyCGC(const CLOCKING_SelectPeripheral_t peripheral, const bool EnorDi)
{
    if (true == EnorDi)
    {
        PCC->CLKCFG[peripheral] |= PCC_CLKCFG_CGC_MASK;
    }
    else
    {
        PCC->CLKCFG[peripheral] &= ~PCC_CLKCFG_CGC_MASK;
    }
}

void CLOCKING_ConfigCGCandPCS(const CLOCKING_SelectPeripheral_t peripheral, const bool EnorDi)
{

    if (true == EnorDi)
    {
        PCC_PORTA_INDEX;
        /*Disable CGC bit before configuring */
        PCC->CLKCFG[peripheral] &= ~PCC_CLKCFG_CGC_MASK;

        /*Reset value of PCS bits and then configuring*/
        PCC->CLKCFG[peripheral] = (PCC->CLKCFG[peripheral] & (~PCC_CLKCFG_PCS_MASK)) | PCC_CLKCFG_PCS(PCC_SOURCE_SELECT_FIRC); /*Select FRIC source for default*/

        /*Enable CGC bit */
        PCC->CLKCFG[peripheral] |= PCC_CLKCFG_CGC_MASK;
    }
    else
    {
        PCC->CLKCFG[peripheral] &= ~PCC_CLKCFG_CGC_MASK; /*Turn off clock*/
    }
}

uint32_t CLOCKING_GetClockOfPeripheral(void)
{
    uint32_t returnValue = 0; /*Return value*/
    uint8_t div2 = 0;

    div2 = ((SCG->FIRCDIV & SCG_FIRCDIV_FIRCDIV2_MASK) >> SCG_FIRCDIV_FIRCDIV2_SHIFT);

    if (0 != div2) /*If the peripheral has been clocked*/
    {
        returnValue = CPU_INT_FAST_CLK_HZ / div2;
    }
    else
    {
        /*Do nothing*/
    }

    return returnValue;
}