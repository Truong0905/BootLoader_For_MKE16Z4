#ifndef __QUEUE_H__
#define __QUEUE_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** Max character per line
 * A line has 1 character S, 1 character type, 2 characters ByteCount (corresponding to 1 byte)
 * With 1 byte of bytecount can check up to 255 bytes after it corresponds to 510 ASCII characters
 * In addition, there are 2 characters \r and \ n
 * So in 1 line can have up to 516 characters
 * That's why we choose 1024 as the max size because it is equal to 2^10
 * */
#define QUEUE_MAX_CHARACTER_PER_LINE (1024u)

/**
 * @brief   The data of a queue includes the characters of a line and the total number of characters
 */
typedef struct
{
    uint8_t data[QUEUE_MAX_CHARACTER_PER_LINE];
    uint16_t length;
} QUEUE_struct_t;
/**
 * @}
 */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief  Init Queue
 * @return none
 */
void QUEUE_Init(void);

/**
 * @brief  Confirm receipt of 1 queue and increase front variable by 1
 * @return none
 */
void QUEUE_EnQueue(void);

/**
 * @brief  Confirm that 1 queue has been taken and increment the rear variable by 1
 * @return none
 */
void QUEUE_DeQueue(void);

/**
 * @brief  Receive 1 byte of data and write to array of 1 queue
 * @return none
 */
void QUEUE_PushData(const uint8_t data);

/**
 * @brief  Get 1 queue
 * @return QUEUE_struct_t Returns the address of a queue
 */
QUEUE_struct_t *QUEUE_PopData(void);

bool QUEUE_IsEmpty(void);

/**
 * @brief  Check if full
 * @return bool Otherwise full will return false
 */
bool QUEUE_IsFull(void);

#endif /*__QUEUE_H__*/
