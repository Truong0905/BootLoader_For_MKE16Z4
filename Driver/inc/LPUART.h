#ifndef __LPUART_H__
#define __LPUART_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief   LPUART select
 */
typedef enum
{
    LPUART_0 = 0,
    LPUART_1 = 1u,
    LPUART_2 = 2u
} LPUART_Select_t;
/**
 * @}
 */

/**
 * @brief   Configure number of data bits for LPUART
 */
typedef enum
{
    LPUART_7Bits = 0u,
    LPUART_8Bits = 1u,
    LUART_9Bits = 2u,
    LPUART_10Bits = 3u
} LPUART_Length_t;
/**
 * @}
 */

/**
 * @brief   Configure parity of data  for LPUART
 */
typedef enum
{
    LPUART_even = 0u,
    LPUART_odd = 1u,
    LPUART_noParity = 2u
} LPUART_Parity_t;
/**
 * @}
 */

/**
 * @brief  Configure first bit of data bits  for LPUART
 */
typedef enum
{
    LPUART_msbFirst = 0u,
    LPUART_lsbFirst = 1u
} LPUART_FirstBit_t;
/**
 * @}
 */

/**
 * @brief  Configure invert  of data bits  for LPUART
 */
typedef enum
{
    LPUART_inversionDisable = 0u,
    LPUART_inversionEnable = 1u
} LPUART_Inversion_t;
/**
 * @}
 */

/**
 * @brief   Select number of stop bits
typedef enum
{
    LPUART_1BitStop = 0u,
    LPUART_2BitsStop = 1u
} LPUART_BitsStop_t;
/**
 * @}
 */
 */

/*Using for Rx Callback*/
typedef void (*pRxCallBack)(const uint8_t data);

/*Using for Tx Callback*/
typedef void (*pTxCallBack)(const uint32_t sentLength); /*Amount of data left to transfer ( byte)*/

/**
 * @brief Handle structure for LPUARTx peripheral
 */
typedef struct
{
    LPUART_Select_t lpuartX; /*!< Specifies the LPUART to config.*/
    uint8_t reserver[3];     /*Avoid alignment error*/
    uint32_t clockForLPUART;
    uint32_t baud;              /*!< Specifies the baud rate  for the LPUART.*/
    LPUART_Length_t dataLength; /*!< Specifies the lenght of data bits for the LPUART. */
    LPUART_Parity_t parity;     /*!< Specifies the parity for the LPUART.*/
    LPUART_BitsStop_t stopBits; /*!< Specifies the number of stop bits for the LPUART.*/
    LPUART_FirstBit_t lsbMsb;   /*!< Specifies the first bit to trans or recieve for the LPUART.*/
    LPUART_Inversion_t invert;  /*!< Specifies the inversion for the LPUART. */
    bool txEnable;              /*If true then enable tx*/
    bool rxEnable;              /*If true then enable rx*/
    /*Interrupt config*/
    bool controlInterrupt;  /*If it is true, it will be possible to use the interrupt transmit and receive functions*/
    pTxCallBack txCallBack; /*callback function for transmission interrupt. If not used, assign it with NULL*/
    pRxCallBack rxCallBack; /*callback function for receive interrupt. If not used, assign it with NULL*/
} LPUART_Config_struct_t;
/**
 * @}
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  LPUART initialization and Configuration
 * @param[in] pLPUARTConfig    Contains configuration
 * @return none
 */
void LPUART_Init(const LPUART_Config_struct_t *const pLPUARTConfig);

/**
 * @brief  Send data by interrupt
 * @param[in] lpuartX    LPUART used to send
 * @param[in] buffer    Buffer address to send
 * @param[in] len    Length of buffer
 * @return none
 */
void LPUART_SendDataIT(const LPUART_Select_t lpuartX, uint8_t *const buffer, const uint32_t len);

/**
 * @brief  Send data
 * @param[in] lpuartX    LPUART used to send
 * @param[in] buffer    Buffer address to send
 * @param[in] len    Length of buffer
 * @return none
 */
void LPUART_SendData(const LPUART_Select_t lpuartX, uint8_t *buffer, const uint32_t len);

/**
 * @brief  Recieve data by interrupt
 * @param[in] lpuartX    LPUART used to recieve
 * @return none
 */
void LPUART_RecieveDataIT(const LPUART_Select_t lpuartX);

#endif /*__LPUART_H__*/
