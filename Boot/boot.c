/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>
#include "MKE16Z4.h"
#include "port.h"
#include "fgpio.h"
#include "clocking.h"
#include "LPUART.h"
#include "FLASH.h"
#include "queue.h"
#include "srec.h"
#include "boot.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define BOOT_BAUD_RATE (115200u) /*Baund rate of UART*/
#define BOOT_APP_ADDRESS_FIRST_SECTOR (0x0000A000)
#define BOOT_APP_SUM_SECTOR (23u)
#define BOOT_LOADING_MESSAGE ("Loading...\r\n")

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t s_Succes[] = "Update successful.Please reset board without boot button pressing to run new APP\r\n";
static uint8_t s_Error[] = "Update failed.Press the reset button and press the boot button to enter the boot program again\r\n";
static uint8_t s_Start[] = "Start Update App.Please send SREC file\r\n";
static SREC_Status_t s_statusSREC = SREC_unknow;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Callback function for receive interrupt
 * @param[in] data    Data received
 * @return none
 */
static void BOOT_DataReceivedHandler(const uint8_t data);

/**
 * @brief  Initialize LPUART0 for boot program
 * @return none
 */
static void BOOT_InitUART(void);

/************************************************************************************
 * Static function
 *************************************************************************************/

static void BOOT_InitUART(void)
{
    PORT_Config_struct_t portT0;
    PORT_Config_struct_t portR0;
    LPUART_Config_struct_t lpuart0;

    /*Init clock for Port and LPUART0*/
    CLOCKING_FIRCDiv2Config(SGC_div1);
    CLOCKING_ConfigCGCandPCS(CLOCKING_lpuart0, true);
    CLOCKING_ConfigOnlyCGC(CLOCKING_portB, true);

    /*Init Tx*/
    portT0.portX = PORT_B;
    portT0.pin = PORT_pin1;
    portT0.mode = PORT_alternative2;
    portT0.pull = PORT_noPull;
    PORT_Init(&portT0);

    /*Init Rx*/
    portR0.portX = PORT_B;
    portR0.pin = PORT_pin0;
    portR0.mode = PORT_alternative2;
    portR0.pull = PORT_noPull;
    PORT_Init(&portR0);

    /*Init LPUART0*/
    lpuart0.lpuartX = LPUART_0;
    lpuart0.baud = BOOT_BAUD_RATE;
    lpuart0.invert = LPUART_inversionDisable;
    lpuart0.stopBits = LPUART_1BitStop;
    lpuart0.dataLength = LPUART_8Bits;
    lpuart0.lsbMsb = LPUART_lsbFirst;
    lpuart0.parity = LPUART_noParity;
    lpuart0.clockForLPUART = CLOCKING_GetClockOfPeripheral();
    lpuart0.txEnable = true;
    lpuart0.rxEnable = true;
    lpuart0.controlInterrupt = true;
    lpuart0.rxCallBack = BOOT_DataReceivedHandler;
    lpuart0.txCallBack = NULL;
    LPUART_Init(&lpuart0);
}

static void BOOT_DataReceivedHandler(const uint8_t data)
{
    static bool s_checkNewQueue = true; /*Save Isfull test state*/

    if (false == s_checkNewQueue || false == QUEUE_IsFull())
    {
        if (0x0A == data) /*'\n'*/
        {
            QUEUE_PushData(data);
            QUEUE_EnQueue();
            s_checkNewQueue = true;
        }
        else
        {
            s_checkNewQueue = false;
            QUEUE_PushData(data);
        }
    }
}

/*******************************************************************************
 * Code
 ******************************************************************************/

bool BOOT_Init(void)
{
    bool check = true;

    QUEUE_Init();
    s_statusSREC = SREC_Init();
    BOOT_InitUART();

    /*Disable all interrupts before working with FLASH*/
    __disable_irq();
    check = FLASH_EraseMultiSector(BOOT_APP_ADDRESS_FIRST_SECTOR, BOOT_APP_SUM_SECTOR);
    __enable_irq();
    /*********/

    if (true == check)
    {
        LPUART_SendDataIT(LPUART_0, s_Start, sizeof(s_Start) - 1);
        LPUART_RecieveDataIT(LPUART_0);
    }
    else
    {
        LPUART_SendDataIT(LPUART_0, s_Error, sizeof(s_Error) - 1);
    }

    return check;
}

void BOOT_InitButton(void)
{
    FGPIO_Config_struct_t buttonFGPIO;
    PORT_Config_struct_t buttonPORT;

    CLOCKING_ConfigOnlyCGC(CLOCKING_portD, true);

    buttonFGPIO.fgpioX = FGPIO_D;
    buttonFGPIO.pin = FGPIO_pin2;
    buttonFGPIO.outputMode = false;
    FGPIO_Init(&buttonFGPIO);

    buttonPORT.portX = PORT_D;
    buttonPORT.pin = PORT_pin2;
    buttonPORT.mode = PORT_gpio;
    buttonPORT.pull = PORT_noPull;
    PORT_Init(&buttonPORT);
}

bool BOOT_PressButton(void)
{
    return (false == FGPIO_ReadPin(FGPIO_D, FGPIO_pin2));
}

bool BOOT_Task(void)
{
    QUEUE_struct_t *dataQueue = NULL; /*Data per line of SREC file*/
    SREC_Data_struct_t dataSrec;
    uint32_t i = 0;
    bool check = true;

    if ((false == QUEUE_IsEmpty()))
    {
        dataQueue = QUEUE_PopData();

        LPUART_SendData(LPUART_0, BOOT_LOADING_MESSAGE, sizeof(BOOT_LOADING_MESSAGE) - 1);
        s_statusSREC = SREC_Parse(dataQueue->data, dataQueue->length, &dataSrec);
        QUEUE_DeQueue();

        if ((SREC_inData == s_statusSREC)) /*This is the data that needs to be saved to FLASH*/
        {
            if (dataSrec.address >= BOOT_APP_ADDRESS_FIRST_SECTOR)
            {
                for (i = 0; i < dataSrec.length; i += 4)
                {
                    /*Disable all interrupts before working with FLASH*/
                    __disable_irq();
                    check = FLASH_ProgramLongWord((dataSrec.address + i), &dataSrec.data[i]);
                    __enable_irq();
                    /*********/

                    if (false == check)
                    {
                        break;
                    }
                }
            }
            else
            {
                check = false;
            }
        }
    }

    if (SREC_success == s_statusSREC)
    {
        SREC_Deinit();
        LPUART_SendData(LPUART_0, s_Succes, sizeof(s_Succes) - 1);
        check = false;
    }
    else if ((SREC_error == s_statusSREC) || ((false == check)))
    {
        LPUART_SendData(LPUART_0, s_Error, sizeof(s_Error) - 1);
        check = false;
    }
    else
    {
        /*do nothing*/
    }

    return check;
}
void BOOT_APP_Task(void)
{
    void (*resetHandler)(void) = NULL;
    /*Disable all interrupt */
    __disable_irq();
    /*Set main stack pointer*/
    SCB->VTOR = (uint32_t)BOOT_APP_ADDRESS_FIRST_SECTOR;
    __enable_irq();
    /*Set new MSP*/
    __set_MSP(*((volatile uint32_t *)BOOT_APP_ADDRESS_FIRST_SECTOR));
    /*Set program counter*/
    resetHandler = (void (*)(void))(*(volatile uint32_t *)(BOOT_APP_ADDRESS_FIRST_SECTOR + 4));
    /*Jump to app*/
    resetHandler();
}
