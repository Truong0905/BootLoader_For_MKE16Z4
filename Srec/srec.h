#ifndef __SREC_H__
#define __SREC_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**A line has 1 byte for bytecount so 255 bytes can be checked after it (Include address, data, checksum)
 * The minimum size of the address is 2 bytes. Checksum has 1 byte.
 * Therefore, the maximum length of data is 252 bytes. We choose the maximum data size to be 256 bytes ( = 2^8)
 */
#define SREC_DATA_MAX_SIZE (256u)

/**
 * @brief   Srec file check status
 */
typedef enum
{
    SREC_start = 0x00,         /*Check S0*/
    SREC_inProgress = 0x01,    /*The intermediate state goes from parsing S0 to S1,2,3 or from S5,6 to S1,2,3*/
    SREC_inData = 0x02,        /*Parsing S1,2,3*/
    SREC_inCount = 0x03,       /*Parsing S5,6*/
    SREC_inTermination = 0x04, /*Parsing S 7,8,9*/
    SREC_error = 0x05,         /*error*/
    SREC_success = 0x06u,      /*Notice that the Srec file has been successfully checked and has no errors*/
    SREC_unknow = 0xff         /*unknow*/
} SREC_Status_t;
/**
 * @}
 */

/**
 * @brief Get the address and data fields of a line . Also store the size of the data field (in bytes)
 */
typedef struct
{
    uint32_t address;
    uint8_t data[SREC_DATA_MAX_SIZE];
    uint16_t length;
} SREC_Data_struct_t;
/**
 * @}
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief   Initialize the SREC file parse
 * @return SREC_Status_t  Returns parse state S0
 */
SREC_Status_t SREC_Init(void);

/**
 * @brief  Parse SREC file
 * @param[in] rawData     Initial data (Data of any 1 line)
 * @param[in] lenghtOfLine     length of this line
 * @param[out] outputData     Address field and data field (include length of data) received after parsing
 * @return SREC_Status_t Returns parsing status
 */
SREC_Status_t SREC_Parse(const uint8_t *const rawData, const uint16_t lenghtOfLine, SREC_Data_struct_t *const outputData);

/**
 * @brief  Deinit parse SREC file
 * @return none
 */
void SREC_Deinit(void);

#endif /*__SREC_H__*/
