/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include "MKE16Z4.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FLASH_CMD_PROGRAM_LONGWORD (0x06)   /*command to program longword*/
#define FLASH_CMD_ERASE_FLASH_SECTOR (0x09) /*Command to erase 1 flash sector*/
#define FLASH_SECTOR_SIZE (1024u)           /*Size per sector (byte)*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Erase all bytes in a program flash sector.
 * @param[in] address    Address to erase
 * @return none
 */
static void FLASH_EraseSector(const uint32_t address);
/*******************************************************************************
 * Static Function
 ******************************************************************************/

static void FLASH_EraseSector(const uint32_t address)
{
    /*Wait previous command complete*/
    while ((1 != ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) >> FTFA_FSTAT_CCIF_SHIFT)));
    /*Access Error and Protection Violation Check*/
    if ((1 == ((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) >> FTFA_FSTAT_FPVIOL_SHIFT)) || (1 == ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) >> FTFA_FSTAT_ACCERR_SHIFT)))
    {
        /*Clear the old errors*/
        FTFA->FSTAT = (FTFA_FSTAT_FPVIOL_MASK | FTFA_FSTAT_ACCERR_MASK);
    }

    /*Write to the FCCOB0 registers command to  erase all bytes in a program flash sector.*/
    FTFA->FCCOB0 = FLASH_CMD_ERASE_FLASH_SECTOR;

    /*Load address*/
    FTFA->FCCOB1 = (uint8_t)(address >> 16);
    FTFA->FCCOB2 = (uint8_t)(address >> 8);
    FTFA->FCCOB3 = (uint8_t)(address >> 0);

    /*Clear the CCIF to launch the command*/
    FTFA->FSTAT = FTFA_FSTAT_CCIF_MASK;

    /*Bit Polling for Command Completion Check*/
    while ((1 != ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) >> FTFA_FSTAT_CCIF_SHIFT)));
}

/*******************************************************************************
 * Codes
 ******************************************************************************/

bool FLASH_ProgramLongWord(const uint32_t address, uint8_t data[4])
{
    bool check = true;

    if (0 == (address % 4)) /*Check that the address where the data is to be written has alignment for 4. */
    {
        /*Wait previous command complete*/
        while ((1 != ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) >> FTFA_FSTAT_CCIF_SHIFT)))
            ;

        /*Access Error and Protection Violation Check*/
        if ((1 == ((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) >> FTFA_FSTAT_FPVIOL_SHIFT)) || (1 == ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) >> FTFA_FSTAT_ACCERR_SHIFT)))
        {
            /*Clear the old errors*/
            FTFA->FSTAT = (FTFA_FSTAT_FPVIOL_MASK | FTFA_FSTAT_ACCERR_MASK);
        }
        /*Write to the FCCOB0 registers command to  program 4 bytes in a program flash block.*/
        FTFA->FCCOB0 = FLASH_CMD_PROGRAM_LONGWORD;

        /*Load address*/
        FTFA->FCCOB1 = (uint8_t)(address >> 16);
        FTFA->FCCOB2 = (uint8_t)(address >> 8);
        FTFA->FCCOB3 = (uint8_t)(address >> 0);

        /*Load data */
        FTFA->FCCOB4 = (uint8_t)(data[3]);
        FTFA->FCCOB5 = (uint8_t)(data[2]);
        FTFA->FCCOB6 = (uint8_t)(data[1]);
        FTFA->FCCOB7 = (uint8_t)(data[0]);

        /*Clear the CCIF to launch the command*/
        FTFA->FSTAT = FTFA_FSTAT_CCIF_MASK;

        /*Bit Polling for Command Completion Check*/
        while ((1 != ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) >> FTFA_FSTAT_CCIF_SHIFT)))
            ;
    }
    else
    {
        check = false;
    }

    return check;
}


bool FLASH_EraseMultiSector(const uint32_t address, const uint16_t size)
{
    uint16_t i;
    bool check = true;

    if (0 == (address % FLASH_SECTOR_SIZE)) /*Check that the address where the data is to be written has alignment for 1 sector (1024 byte). */
    {
        for (i = 0; i < size; i++)
        {
            FLASH_EraseSector(address + i * FLASH_SECTOR_SIZE);
        }
    }
    else
    {
        check = false;
    }

    return check;
}
