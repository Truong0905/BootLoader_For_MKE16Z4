/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stddef.h>
#include <stdbool.h>
#include "MKE16Z4.h"
#include "LPUART.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief structure for data interrupt transmission 
 */
typedef struct
{
    uint8_t *TxBuffer;   /*The buffer address contains the data to be transferred*/
    uint32_t sentLength; /*Amount of data left to transfer ( byte)*/
    bool stop;           /*Send data with an interrupt triggered when this variable is false*/
} LPUART_TxBuffer_struct_t;
/**
 * @}
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/

static pRxCallBack s_RxlpuartCallBack = NULL; /*Call back funtion for Rx*/
static pTxCallBack s_TxlpuartCallBack = NULL; /*Call back funtion for Tx*/
LPUART_Type *s_ArrayLPUART[3] = LPUART_BASE_PTRS;
static LPUART_TxBuffer_struct_t s_TxBuffer;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Find the OSR and SBR such that the input baudrate response
 * @param[in] clockForLPUART    clock of LPUART
 * @param[in] baudRate   input baudrate
 * @param[out] pLPUARTx   Address of LPUARTx
 * @return none
 */
static void LPUART_SetBaudRate(const uint32_t clockForLPUART, const uint32_t baudRate, LPUART_Type *const pLPUARTx);

/**
 * @brief  Disable NVIC interrupt for LPUARTx
 * @param[out] pLPUARTx   Address of LPUARTx
 * @return none
 */
static void LPUART_DisableNVIC(LPUART_Type *pLPUARTx);

/**
 * @brief  Enable NVIC interrupt for LPUARTx
 * @param[out] pLPUARTx   Address of LPUARTx
 * @return none
 */
static void LPUART_EnableNVIC(LPUART_Type *pLPUARTx);

/*******************************************************************************
 * Static Function
 ******************************************************************************/

static void LPUART_SetBaudRate(const uint32_t clockForLPUART, const uint32_t baudRate, LPUART_Type *const pLPUARTx)
{
    uint8_t i = 0;              /*Index value*/
    uint16_t divisionRatio = 0; /*Baud Rate Modulo Divisor*/
    uint16_t tempDivisionRatio = 0;
    uint8_t overSampling = 0; /*Oversampling Ratio*/
    uint32_t tempBaudRate;
    uint32_t errorPre = 0xFFFFFFFF; /*Error baud previous*/
    uint32_t errorCurrent = 0;      /*Error baud current*/
    bool check = false;

    /*Algorithm to find OSR and SBR such that the baudrate error is minimal*/
    for (i = 4; i <= 32; i++) /*Oversampling Ratio is approved from 3 to 31 according to reference*/
    {
        tempDivisionRatio = clockForLPUART / (baudRate * i);
        if (0 == tempDivisionRatio)
        {
            tempDivisionRatio = 1;
            check = false;
        }
        tempBaudRate = clockForLPUART / (tempDivisionRatio * i);

        if (tempBaudRate >= baudRate)
        {
            errorCurrent = tempBaudRate - baudRate;
        }
        else
        {
            errorCurrent = baudRate - tempBaudRate;
        }

        if (0 == errorCurrent)
        {
            overSampling = i - 1;
            divisionRatio = tempDivisionRatio;
            break;
        }
        else if (errorCurrent < errorPre)
        {
            overSampling = i - 1;
            divisionRatio = tempDivisionRatio;
            errorPre = errorCurrent;
            if (true == check)
            {
                tempBaudRate = clockForLPUART / ((divisionRatio + 1) * i);
                if (tempBaudRate >= baudRate)
                {
                    errorCurrent = tempBaudRate - baudRate;
                }
                else
                {
                    errorCurrent = baudRate - tempBaudRate;
                }

                if (errorCurrent < errorPre)
                {
                    divisionRatio++;
                    errorPre = errorCurrent;
                }
                else
                {
                    /*Do nothing*/
                }
            }
        }
        else
        {
            /*do nothing*/
        }
        check = true;
    }

    if ((3 <= overSampling) && ((6 >= overSampling)))
    {
        pLPUARTx->BAUD |= LPUART_BAUD_BOTHEDGE_MASK;
    }

    pLPUARTx->BAUD = (pLPUARTx->BAUD & (~LPUART_BAUD_OSR_MASK)) | LPUART_BAUD_OSR(overSampling);
    pLPUARTx->BAUD = (pLPUARTx->BAUD & (~LPUART_BAUD_SBR_MASK)) | LPUART_BAUD_SBR(divisionRatio);
}

static void LPUART_EnableNVIC(LPUART_Type *pLPUARTx)
{
    if (LPUART0 == pLPUARTx)
    {
        NVIC_EnableIRQ(LPUART0_IRQn);
    }
    else if (LPUART1 == pLPUARTx)
    {
        NVIC_EnableIRQ(LPUART1_IRQn);
    }
    else if (LPUART2 == pLPUARTx)
    {
        NVIC_EnableIRQ(LPUART2_IRQn);
    }
    else
    {
        /*Do nothing*/
    }
}

static void LPUART_DisableNVIC(LPUART_Type *pLPUARTx)
{
    if (LPUART0 == pLPUARTx)
    {
        NVIC_DisableIRQ(LPUART0_IRQn);
    }
    else if (LPUART1 == pLPUARTx)
    {
        NVIC_DisableIRQ(LPUART1_IRQn);
    }
    else if (LPUART2 == pLPUARTx)
    {
        NVIC_DisableIRQ(LPUART2_IRQn);
    }
    else
    {
        /*Do nothing*/
    }
}

/*******************************************************************************
 * Code
 ******************************************************************************/

void LPUART_Init(const LPUART_Config_struct_t *const pLPUARTConfig)
{

    LPUART_Type *pLPUARTx = s_ArrayLPUART[pLPUARTConfig->lpuartX];

    /*Disable TX RX */
    pLPUARTx->CTRL &= ~LPUART_CTRL_RE_MASK;
    pLPUARTx->CTRL &= ~LPUART_CTRL_TE_MASK;

    /*Number of data bits */
    if (LPUART_10Bits == pLPUARTConfig->dataLength)
    {
        pLPUARTx->BAUD |= LPUART_BAUD_M10(1);
    }
    else if (LUART_9Bits == pLPUARTConfig->dataLength)
    {
        pLPUARTx->BAUD &= ~LPUART_BAUD_M10_MASK;
        pLPUARTx->CTRL &= ~LPUART_CTRL_M7_MASK;
        pLPUARTx->CTRL |= LPUART_CTRL_M_MASK;
    }
    else if (LPUART_8Bits == pLPUARTConfig->dataLength)
    {
        pLPUARTx->BAUD &= ~LPUART_BAUD_M10_MASK;
        pLPUARTx->CTRL &= ~LPUART_CTRL_M7_MASK;
        pLPUARTx->CTRL &= ~LPUART_CTRL_M7_MASK;
    }
    else
    {
        pLPUARTx->BAUD &= ~LPUART_BAUD_M10_MASK;
        pLPUARTx->CTRL |= LPUART_CTRL_M7_MASK;
    }

    /*Parity */
    if (LPUART_noParity == pLPUARTConfig->parity)
    {
        pLPUARTx->CTRL &= ~LPUART_CTRL_PE_MASK;
    }
    else if (LPUART_even == pLPUARTConfig->parity)
    {
        pLPUARTx->CTRL |= LPUART_CTRL_PE_MASK;
        pLPUARTx->CTRL &= ~LPUART_CTRL_PT_MASK;
    }
    else if (LPUART_odd == pLPUARTConfig->parity)
    {
        pLPUARTx->CTRL |= (LPUART_CTRL_PT_MASK | LPUART_CTRL_PE_MASK);
    }
    else
    {
        /*Do nothing*/
    }

    /*MSB or LSB first */
    if (LPUART_lsbFirst == pLPUARTConfig->lsbMsb)
    {
        pLPUARTx->STAT &= ~LPUART_STAT_MSBF_MASK;
    }
    else if (LPUART_msbFirst == pLPUARTConfig->lsbMsb)
    {
        pLPUARTx->STAT |= LPUART_STAT_MSBF_MASK;
    }
    else
    {
        /*Do nothing*/
    }

    /*Data invert */
    if (LPUART_inversionEnable == pLPUARTConfig->invert)
    {

        pLPUARTx->CTRL |= LPUART_CTRL_TXINV_MASK;
        pLPUARTx->STAT |= LPUART_STAT_RXINV_MASK;
    }
    else
    {
        pLPUARTx->CTRL &= ~LPUART_CTRL_TXINV_MASK;
        pLPUARTx->STAT &= ~LPUART_STAT_RXINV_MASK;
    }

    /*Configure number of stop bit*/
    if (LPUART_1BitStop == pLPUARTConfig->stopBits)
    {
        pLPUARTx->BAUD &= ~LPUART_BAUD_SBNS_MASK;
    }
    else
    {
        pLPUARTx->BAUD |= LPUART_BAUD_SBNS_MASK;
    }

    /*Configure baud rate*/
    LPUART_SetBaudRate(pLPUARTConfig->clockForLPUART, pLPUARTConfig->baud, pLPUARTx);

    /*Configure mode*/
    if (true == pLPUARTConfig->txEnable)
    {
        pLPUARTx->CTRL |= LPUART_CTRL_TE_MASK;
    }

    if (true == pLPUARTConfig->rxEnable)
    {
        pLPUARTx->CTRL |= LPUART_CTRL_RE_MASK;
    }

    /*Config interrupt*/
    if (true == pLPUARTConfig->controlInterrupt) /*If select*/
    {
        LPUART_EnableNVIC(pLPUARTx);
    }
    else
    {
        LPUART_DisableNVIC(pLPUARTx);
    }
    s_TxBuffer.stop = true;
    s_RxlpuartCallBack = pLPUARTConfig->rxCallBack;
    s_TxlpuartCallBack = pLPUARTConfig->txCallBack;
}
void LPUART_SendDataIT(const LPUART_Select_t lpuartX, uint8_t *const buffer, const uint32_t len)
{
    LPUART_Type *pLPUARTx = s_ArrayLPUART[lpuartX];

    if (1 <= len)
    {
        s_TxBuffer.TxBuffer = buffer + 1;
        s_TxBuffer.sentLength = len;
        pLPUARTx->DATA = buffer[0];
        s_TxBuffer.sentLength--;
        s_TxBuffer.stop = false;
        pLPUARTx->CTRL |= LPUART_CTRL_TIE_MASK;
    }
    else
    {
        /*do nothing*/
    }
}

void LPUART_SendData(const LPUART_Select_t lpuartX, uint8_t *buffer, const uint32_t len)
{
    uint32_t i = 0;
    LPUART_Type *pLPUARTx = s_ArrayLPUART[lpuartX];

    for (i = 0; i < len; i++)
    {
        while (0 == ((pLPUARTx->STAT >> LPUART_STAT_TDRE_SHIFT) & 0x01));/*Wait transmit data buffer empty*/
        pLPUARTx->DATA = (*buffer & (uint8_t)0xFF); /*Send one byte*/
        buffer++;                                   /*Next byte*/
    }
}

void LPUART_RecieveDataIT(const LPUART_Select_t lpuartX)
{
    s_ArrayLPUART[lpuartX]->CTRL |= LPUART_CTRL_RIE_MASK;
}

void LPUART0_IRQHandler(void)
{
    if (1u == ((LPUART0->STAT & LPUART_STAT_RDRF_MASK) >> LPUART_STAT_RDRF_SHIFT))
    {
        s_RxlpuartCallBack((uint8_t)LPUART0->DATA);
    }

    if ((false == s_TxBuffer.stop) && (1u == ((LPUART0->STAT & LPUART_STAT_TDRE_MASK) >> LPUART_STAT_TDRE_SHIFT)))
    {
        if (0 == s_TxBuffer.sentLength)
        {
            s_TxBuffer.stop = true;
            LPUART0->CTRL &= ~LPUART_CTRL_TIE_MASK;
        }
        else
        {
            LPUART0->DATA = *s_TxBuffer.TxBuffer;
            s_TxBuffer.TxBuffer++;
            s_TxBuffer.sentLength--;
        }

        if (NULL != s_TxlpuartCallBack)
        {
            s_TxlpuartCallBack(s_TxBuffer.sentLength);
        }
    }
}