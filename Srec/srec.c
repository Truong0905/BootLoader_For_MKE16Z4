/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "srec.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*
 * Macros of record type
 */
#define SREC_TYPE_S0 (0u)
#define SREC_TYPE_S1 (1u)
#define SREC_TYPE_S2 (2u)
#define SREC_TYPE_S3 (3u)
#define SREC_TYPE_S4 (4u)
#define SREC_TYPE_S5 (5u)
#define SREC_TYPE_S6 (6u)
#define SREC_TYPE_S7 (7u)
#define SREC_TYPE_S8 (8u)
#define SREC_TYPE_S9 (9u)
#define SREC_TYPE_UNKNOWN (10u) /*original type untested*/

/*
 * Position of characters in the ascii
 */
#define SREC_ZERO_ASCII (0x30)          /*Position character 0 in the ascii*/
#define SREC_NINE_ASCII (0x39)          /*Position character 9 in the ascii*/
#define SREC_A_CAPITALIZE_LETTER (0x41) /*The position of the capital letter 'A' in the ascii */
#define SREC_F_CAPITALIZE_LETTER (0x46) /*The position of the capital letter 'F' in the ascii */
#define SREC_S_CAPITALIZE_LETTER (0x53) /*The position of the capital letter 'S' in the ascii */

/*******************************************************************************
 * Variables
 ******************************************************************************/

static SREC_Status_t s_Status = SREC_unknow;          /*Current status of SREC  file parsing*/
static uint16_t s_CountError = 0;                     /*Check if the line has a non-Hexa character (does not check the initial S character)*/
static uint32_t s_CountDataLine = 0;                  /*Count the number of lines S1,S2,S3 in file*/
static uint8_t s_CheckTypeOfData = SREC_TYPE_UNKNOWN; /*Check if this file is using fields S1 or S2,S3*/
static uint8_t s_SizeAddress = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Parse 1 line
 * @param[in] dataSrec     Data of any 1 line (Excluding S and type)
 * @param[in] lenghtOfLine     length of this line (Excluding S and type)
 * @param[out] outputData     Address field and data field (include length of data) received after parsing
 * @return bool Returns true if there is no error
 */
static bool SREC_ParseLine(const uint8_t *const dataSrec, const uint16_t lenghtOfLine, SREC_Data_struct_t *const outputData);

/**
 * @brief  Convert 1 character ASCII To Decimal
 * @param[in] data     Data of any 1 line
 * @return bool Returns true if there is no error
 */
static inline uint8_t SREC_ConvertASCIIToDecima(const uint8_t data);

/************************************************************************************
 * Static function
 *************************************************************************************/

static bool SREC_ParseLine(const uint8_t *const dataSrec, const uint16_t lenghtOfLine, SREC_Data_struct_t *const outputData)
{
    uint8_t i = 0;        /*Index*/
    uint16_t offset = 0;  /*Location to check*/
    uint32_t data = 0;    /*Data after converting from string ascii to decimal*/
    uint32_t address = 0; /*temp address*/
    uint32_t checkSum = 0;
    uint16_t sizeData = 0; /*size of data field (byte)*/
    bool check = true;     /*Return status*/

    /*Check byte count*/
    data = (SREC_ConvertASCIIToDecima(dataSrec[offset]) << 4) | SREC_ConvertASCIIToDecima(dataSrec[offset + 1]);
    checkSum += data;
    if ((2 * data) != (lenghtOfLine - 4)) /* Subtract (2 byte count  characters ,1'\n',1'\r')*/
    {
        check = false;
    }
    /*Calculate address*/
    offset += 2;
    for (i = 0; i < s_SizeAddress; i++)
    {
        data = (SREC_ConvertASCIIToDecima(dataSrec[offset]) << 4) | SREC_ConvertASCIIToDecima(dataSrec[offset + 1]);
        address = (address << 8) | data;
        checkSum += data;
        offset += 2;
    }
    outputData->address = address;

    /** Calculate the total number of data bytes
     * Subtract (1 character byte count + 2 characters checksum + 1 character \r + 1 character \n + 2*sizeAddress character )
     * Then divide by 2 because each byte is 2 characters
     */
    sizeData = (lenghtOfLine - (2 + 2 * s_SizeAddress + 2 + 2)) / 2;
    outputData->length = sizeData;

    /*Save each byte of data*/
    for (i = 0; i < sizeData; i++)
    {
        data = (SREC_ConvertASCIIToDecima(dataSrec[offset]) << 4) | SREC_ConvertASCIIToDecima(dataSrec[offset + 1]);
        offset += 2;
        outputData->data[i] = data;
        checkSum += data;
    }

    /*Check Sum */
    data = (SREC_ConvertASCIIToDecima(dataSrec[offset]) << 4) | SREC_ConvertASCIIToDecima(dataSrec[offset + 1]);
    checkSum ^= 0xFFFFFFFF; /*Invert bit. Because sumbyte is UINT32_T*/
    checkSum &= 0xFF;       /*Get the LSB  byte*/

    if (checkSum != data) /*Compare with checksum*/
    {
        check = false;
    }
    if (0 != s_CountError)
    {
        check = false; /*There are non-hexa characters in line*/
    }

    return check;
}

static inline uint8_t SREC_ConvertASCIIToDecima(const uint8_t data)
{
    uint8_t valueReturn = 0; /*Variable containing return value*/

    /*Are the characters A-F*/
    if ((SREC_F_CAPITALIZE_LETTER >= data) && (SREC_A_CAPITALIZE_LETTER <= data))
    {
        valueReturn = data - SREC_A_CAPITALIZE_LETTER + 10;
    }
    /*Are the characters 0-9*/
    else if (SREC_NINE_ASCII >= data && (SREC_ZERO_ASCII <= data))
    {
        valueReturn = data - SREC_ZERO_ASCII;
    }
    else
    {
        s_CountError = 1; /*There are strange characters*/
    }

    return valueReturn;
}

/*******************************************************************************
 * Code
 ******************************************************************************/

SREC_Status_t SREC_Init(void)
{
    s_Status = SREC_start;
    s_CountDataLine = 0;
    s_CountError = 0;
    s_CheckTypeOfData = SREC_TYPE_UNKNOWN;
    s_SizeAddress = 0;

    return s_Status;
}

SREC_Status_t SREC_Parse(const uint8_t *const rawData, const uint16_t lenghtOfLine, SREC_Data_struct_t *const outputData)
{
    uint16_t offset = 0;
    uint32_t data = 0;
    bool checkLine = true;

    if (SREC_unknow == s_Status)
    {
        /*do nothing*/
    }
    else if ((SREC_S_CAPITALIZE_LETTER == rawData[offset]))
    {
        offset++;
    }
    else
    {
        s_Status = SREC_error;
    }

    data = SREC_ConvertASCIIToDecima(rawData[offset]); /*Save type*/
    offset++;

    switch (s_Status)
    {
    case SREC_unknow:
        break;
    case SREC_start:
    {
        if (SREC_TYPE_S0 == data) /*The first line of the file must be S0*/
        {
            s_SizeAddress = 2; /*Number byte address of this type*/

            checkLine = SREC_ParseLine(&rawData[offset], lenghtOfLine - 2, outputData); /*Subtract S and type*/

            /*For line S0, the address field must be 0 */
            if ((0 != outputData->address) || (false == checkLine))
            {
                s_Status = SREC_error;
            }

            if (SREC_error != s_Status)
            {
                s_Status = SREC_inProgress;
            }
        }
        else
        {
            s_Status = SREC_error;
        }
        break;
    }
    case SREC_inProgress:
        s_Status = SREC_inData;
    case SREC_inData:
    {
        if ((SREC_TYPE_S1 <= data) && (SREC_TYPE_S3 >= data))
        {
            /*Check in this file there is more than 1 type S1 or S2 or 3*/
            if ((SREC_TYPE_UNKNOWN != s_CheckTypeOfData) && (s_CheckTypeOfData != data))
            {
                s_Status = SREC_error;
            }
            else
            {
                s_CheckTypeOfData = data;                                                   /*save type data this file*/
                s_CountDataLine++;                                                          /*Increase the number of data lines*/
                s_SizeAddress = data + 1;                                                   /*Number byte address of this type*/
                checkLine = SREC_ParseLine(&rawData[offset], lenghtOfLine - 2, outputData); /*Subtract S and type*/
                if (false == checkLine)
                {
                    s_Status = SREC_error;
                }
            }
            break;
        }
        else if (((SREC_TYPE_S5 == data) || (SREC_TYPE_S6 == data)) || (((SREC_TYPE_S7 <= data) && (SREC_TYPE_S9 >= data))))
        {
            s_Status = SREC_inCount; /*Switch to inCount*/
        }

        else
        {
            s_Status = SREC_error;
            break;
        }
    }
    case SREC_inCount:
    {
        if ((SREC_TYPE_S7 <= data) && (SREC_TYPE_S9 >= data))
        {
            s_Status = SREC_inTermination; /*Switch to inTermination*/
        }
        else
        {
            s_SizeAddress = data - 3;                                                   /*Number byte address of this type*/
            checkLine = SREC_ParseLine(&rawData[offset], lenghtOfLine - 2, outputData); /*Subtract S and type*/
            if (false != checkLine)
            {
                if ((s_CountDataLine != outputData->address)) /*Check tra number line S1 or S2 or S3 in File*/
                {
                    s_Status = SREC_error;
                }
                else
                {
                    s_Status = SREC_inProgress;
                }
            }
            else
            {
                s_Status = SREC_error;
            }
            break;
        }
    }
    case SREC_inTermination:
    {
        /*If it is S9, then the  type is S1 */
        if ((SREC_TYPE_S1 == s_CheckTypeOfData) && (SREC_TYPE_S9 == data))
        {
            /*Do nothing*/
        }
        /*If it is S8, then the  type is S2 */
        else if ((SREC_TYPE_S2 == s_CheckTypeOfData) && (SREC_TYPE_S8 == data))
        {
            /*Do nothing*/
        }
        /*If it is S7, then the  type is S3 */
        else if ((SREC_TYPE_S3 == s_CheckTypeOfData) && (SREC_TYPE_S7 == data))
        {
            /*Do nothing*/
        }
        else
        {
            s_Status = SREC_error;
        }
        if (SREC_error != s_Status)
        {
            s_SizeAddress = 11 - data;                                                  /*Number byte address of this type*/
            checkLine = SREC_ParseLine(&rawData[offset], lenghtOfLine - 2, outputData); /*Subtract S and type*/
            if (false != checkLine)
            {
                s_Status = SREC_success;
            }
            else
            {
                s_Status = SREC_error;
            }
        }
        break;
    }
    default:
        s_Status = SREC_error;
        break;
    }

    return s_Status;
}

void SREC_Deinit(void)
{
    s_Status = SREC_unknow;
}
