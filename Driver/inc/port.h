#ifndef __PORT_H__
#define __PORT_H__

/*******************************************************************************
 * Defininterruptions
 ******************************************************************************/

/**
 * @brief   Select PORT
 */
typedef enum
{
    PORT_A = 0u,
    PORT_B = 1u,
    PORT_C = 2u,
    PORT_D = 3u,
    PORT_E = 4u

} PORT_Select_t;
/**
 * @}
 */

/**
 * @brief   Select mode
 */
typedef enum
{
    PORT_disablePin = 0u,
    PORT_gpio = 1u,
    PORT_alternative2 = 2u,
    PORT_alternative3 = 3u,
    PORT_alternative4 = 4u,
    PORT_alternative5 = 5u,
    PORT_alternative6 = 6u,
    PORT_alternative7 = 7u,
    /* Interrupt mode */
    PORT_interrupt_Logic0 = 8u,  /* ISF flag and Interrupt when logic 0. */
    PORT_interrupt_RT = 9u,      /*  ISF flag and Interrupt on rising-edge */
    PORT_interrupt_FT = 10u,     /* ISF flag and Interrupt on falling-edge. */
    PORT_interrupt_RFT = 11u,    /*  ISF flag and Interrupt on either edge */
    PORT_interrupt_Logic1 = 12u, /* ISF flag and Interrupt when logic 1 */

} PORT_Mode_t;
/**
 * @}
 */

/**
 * @brief   Select pull
 */
typedef enum
{
    PORT_pullDown = 0u,
    PORT_pullUp = 1u,
    PORT_noPull = 2u
} PORT_Pull_t;
/**
 * @}
 */

/**
 * @brief   Select pin
 */
typedef enum
{
    PORT_pin0 = 0u,
    PORT_pin1 = 1u,
    PORT_pin2 = 2u,
    PORT_pin3 = 3u,
    PORT_pin4 = 4u,
    PORT_pin5 = 5u,
    PORT_pin6 = 6u,
    PORT_pin7 = 7u,
    PORT_pin8 = 8u,
    PORT_pin9 = 9u,
    PORT_pin10 = 10u,
    PORT_pin11 = 11u,
    PORT_pin12 = 12u,
    PORT_pin13 = 13u,
    PORT_pin14 = 14u,
    PORT_pin15 = 15u,
    PORT_pin16 = 16u,
    PORT_pin17 = 17u,
    PORT_pin18 = 18u,
    PORT_pin19 = 19u,
    PORT_pin20 = 20u,
    PORT_pin21 = 21u,
    PORT_pin22 = 22u,
    PORT_pin23 = 23u,
    PORT_pin24 = 24u,
    PORT_pin25 = 25u,
    PORT_pin26 = 26u,
    PORT_pin27 = 27u,
    PORT_pin28 = 28u,
    PORT_pin29 = 29u,
    PORT_pin30 = 30u,
    PORT_pin31 = 31u
} PORT_pin_t;
/**
 * @}
 */

/**
 * @brief   PORT structure config
 */
typedef struct
{
    PORT_pin_t pin;      /*!< Specifies the  pins to be configured.*/
    PORT_Select_t portX; /*!< Specifies the PORT to config. */
    PORT_Mode_t mode;    /*!< Specifies the operating mode for the selected pins. */
    PORT_Pull_t pull;    /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.*/
} PORT_Config_struct_t;
/**
 * @}
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  PORT pin ininterruptialization and Configuration
 * @param[in] pPORTConfig    Contains Configuration
 * @return none
 */
void PORT_Init(PORT_Config_struct_t *const pPORTConfig);

#endif /*__PORT_H__*/
