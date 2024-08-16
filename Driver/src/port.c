/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stddef.h>
#include "MKE16Z4.h"
#include "port.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

void PORT_Init(PORT_Config_struct_t *const pPORTConfig)
{
  PORT_Type *arrayPORT[5] = PORT_BASE_PTRS;
  uint8_t pinSelect = 0;
  PORT_Type *pPORTx = arrayPORT[pPORTConfig->portX];

  pinSelect = pPORTConfig->pin;

  /*Pull configuration is valid in all digital pin muxing modes*/
  if (PORT_pullDown == pPORTConfig->pull)
  {
    pPORTx->PCR[pinSelect] |= PORT_PCR_PE(1);
    pPORTx->PCR[pinSelect] &= ~PORT_PCR_PS(1);
  }
  else if (PORT_pullUp == pPORTConfig->pull)
  {
    pPORTx->PCR[pinSelect] |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
  }
  else
  {
    pPORTx->PCR[pinSelect] &= ~PORT_PCR_PE(1);
  }

  /*Pin Mux Control*/
  if (PORT_alternative7 >= pPORTConfig->mode)
  {
    /* non-Interrupt mode */
    pPORTx->PCR[pinSelect] = (pPORTx->PCR[pinSelect] & (~PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(pPORTConfig->mode);
  }
  else
  {
    /*Select GPIO */
    pPORTx->PCR[pinSelect] = (pPORTx->PCR[pinSelect] & (~PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(PORT_gpio);
    /* Interrupt mode */
    pPORTx->ISFR |= 1 << pinSelect; /*Clear interrupt flag*/
    pPORTx->PCR[pinSelect] = (pPORTx->PCR[pinSelect] & (~PORT_PCR_IRQC_MASK)) | PORT_PCR_IRQC(pPORTConfig->mode);
  }
}
