#ifndef __FLASH_H__
#define __FLASH_H__

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Program 4 bytes in a program flash block.
 * @param[in] address    Address to write data
 * @param[in] data    data
 * @return bool Returns true if write was successful
 */
bool FLASH_ProgramLongWord(const uint32_t address, uint8_t data[4]);

/**
 * @brief  Erase mutil sector.
 * @param[in] address    Address of first sector to erase
 * @return bool Returns true if erase was successful
 */
bool FLASH_EraseMultiSector(const uint32_t address, const uint16_t size);

#endif /*__FLASH_H__*/
