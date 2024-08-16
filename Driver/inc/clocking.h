#ifndef __CLOCKING_H__
#define __CLOCKING_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief   Select peripheral
 */
typedef enum
{
    CLOCKING_portA = 73u,
    CLOCKING_portB = 74u,
    CLOCKING_portC = 75u,
    CLOCKING_portD = 76u,
    CLOCKING_portE = 77u,
    CLOCKING_lpuart0 = 106u,
    CLOCKING_lpuart1 = 107u,
    CLOCKING_lpuart2 = 108u

} CLOCKING_SelectPeripheral_t;
/**
 * @}
 */

/**
 * @brief Dividing options of Fast IRC Clock Divide 2
 */
typedef enum
{
    SGC_fastDiv2Disable = 0u,
    SGC_div1 = 1u,
    SGC_div2 = 2u,
    SGC_div4 = 3u,
    SGC_div8 = 4u,
    SGC_div16 = 5u,
    SGC_div32 = 6u,
    SGC_div64 = 7u
} CLOCKING_SGC_FircDiv2_t;
/**
 * @}
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Used to change clock divider 2 for the Fast IRC. Used by modules that need an asynchronous clock source
 * @param[in] div2Value    Divider value
 * @return none
 */
void CLOCKING_FIRCDiv2Config(const CLOCKING_SGC_FircDiv2_t div2Value);

/**
 * @brief  Used to turn on or off the clock for any peripheral that is configured by CGC
 * @param[in] peripheral    Peripherals need to configure clock
 * @param[in] EnorDi   Enable or Disable
 * @return none
 */
void CLOCKING_ConfigOnlyCGC(const CLOCKING_SelectPeripheral_t peripheral, const bool EnorDi);

/**
 * @brief  Used to turn on or off the clock for any peripheral that is configured by PCS and CGC
 * @param[in] index    Peripherals need to configure clock
 * @param[in] EnorDi Enable or Disable
 * @return none
 */
void CLOCKING_ConfigCGCandPCS(const CLOCKING_SelectPeripheral_t peripheral, const bool EnorDi);

/**
 * @brief Get clock of peripheral
 * @return uint32_t The value of the clock pulse supplied to the peripheral.If not enabled, it will be 0
 */
uint32_t CLOCKING_GetClockOfPeripheral(void);

#endif /*__CLOCKING_H__*/
