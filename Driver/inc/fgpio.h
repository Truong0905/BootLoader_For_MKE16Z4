#ifndef __FGPIO_H__
#define __FGPIO_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief   Select FGPIO
 */
typedef enum
{
    FGPIO_A = 0u,
    FGPIO_B = 1u,
    FGPIO_C = 2u,
    FGPIO_D = 3u,
    FGPIO_E = 4u

} FGPIO_Select_t;
/**
 * @}
 */

/**
 * @brief   Select pin
 */
typedef enum
{
    FGPIO_pin0 = 0u,
    FGPIO_pin1 = 1u,
    FGPIO_pin2 = 2u,
    FGPIO_pin3 = 3u,
    FGPIO_pin4 = 4u,
    FGPIO_pin5 = 5u,
    FGPIO_pin6 = 6u,
    FGPIO_pin7 = 7u,
    FGPIO_pin8 = 8u,
    FGPIO_pin9 = 9u,
    FGPIO_pin10 = 10u,
    FGPIO_pin11 = 11u,
    FGPIO_pin12 = 12u,
    FGPIO_pin13 = 13u,
    FGPIO_pin14 = 14u,
    FGPIO_pin15 = 15u,
    FGPIO_pin16 = 16u,
    FGPIO_pin17 = 17u,
    FGPIO_pin18 = 18u,
    FGPIO_pin19 = 19u,
    FGPIO_pin20 = 20u,
    FGPIO_pin21 = 21u,
    FGPIO_pin22 = 22u,
    FGPIO_pin23 = 23u,
    FGPIO_pin24 = 24u,
    FGPIO_pin25 = 25u,
    FGPIO_pin26 = 26u,
    FGPIO_pin27 = 27u,
    FGPIO_pin28 = 28u,
    FGPIO_pin29 = 29u,
    FGPIO_pin30 = 30u,
    FGPIO_pin31 = 31u
} FGPIO_pin_t;
/**
 * @}
 */

/**
 * @brief   FGPIO  structure config
 */
typedef struct
{
    FGPIO_pin_t pin;       /*!< Specifies the  pins to be configured.*/
    FGPIO_Select_t fgpioX; /*!< Specifies the FGPIO to config. */
    bool outputMode;       /*If true, it will be output mode*/
} FGPIO_Config_struct_t;
/**
 * @}
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  FGPIO  configuration
 * @param[in] pFGPIOConfig     Contains the configuration of the pin
 * @return none
 */
void FGPIO_Init(const FGPIO_Config_struct_t *const pFGPIOConfig);

/**
 * @brief  Set pin
 * @param[in] fgpioX     Designated FGPIO
 * @param[in] pin    Pin to set
 * @return none
 */
void FGPIO_WriteToSetPin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin);

/**
 * @brief  clear pin
 * @param[in] fgpioX     Designated FGPIO
 * @param[in] pin    Pin to clear
 * @return none
 */
void FGPIO_WriteToClearPin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin);

/**
 * @brief  Toggle pin
 * @param[in] fgpioX     Designated FGPIO
 * @param[in] pin    Pin to toggle
 * @return none
 */
void FGPIO_TogglePin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin);

/**
 * @brief  Read pin
 * @param[in] fgpioX     Designated FGPIO
 * @param[in] pin    Pin to read
 * @return bool  Returns true if logical 1
 */
bool FGPIO_ReadPin(const FGPIO_Select_t fgpioX, const FGPIO_pin_t pin);

#endif /*__FGPIO_H__*/
