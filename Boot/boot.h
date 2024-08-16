#ifndef __BOOT_H__
#define __BOOT_H__

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Initialize Queue , SREC and LPUART to proceed with boot
 * In addition, it also deletes the memory area of ​​the old app first
 * @return bool Returns true if initialization was successful
 */
bool BOOT_Init(void);
/**
 * @brief  Run the boot program
 * @return bool If there is an error or successful boot will return false
 */
bool BOOT_Task(void);

/**
 * @brief  Initialize the button to check whether the user will enter the boot program or not
 * @return none
 */
void BOOT_InitButton(void);

/**
 * @brief  Test user's button press selection
 * @return bool If the button is pressed, it will return true
 */
bool BOOT_PressButton(void);

/**
 * @brief  Run app
 * @return none
 */
void BOOT_APP_Task(void);
#endif /*__BOOT_H__*/
