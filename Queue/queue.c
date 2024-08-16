/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*Max queue*/
#define QUEUE_MAX_QUEUE (4u)

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t s_Front;
static uint8_t s_Rear;
static uint16_t s_Count; /*Location of adding data to the array (corresponding to the queue being considered)*/
static uint32_t s_QueueLevel;
static QUEUE_struct_t s_Queue[QUEUE_MAX_QUEUE];

/*******************************************************************************
 * Code
 ******************************************************************************/

void QUEUE_Init(void)
{
    s_Front = 0;
    s_Rear = 0;
    s_Count = 0;
    s_QueueLevel = 0;
}

bool QUEUE_IsFull(void)
{
    return (QUEUE_MAX_QUEUE == s_QueueLevel);
}

bool QUEUE_IsEmpty(void)
{
    return (0 == s_QueueLevel);
}

void QUEUE_EnQueue(void)
{
    s_Queue[s_Rear].length = s_Count;
    s_Rear++;
    s_Rear = s_Rear % QUEUE_MAX_QUEUE;
    s_Count = 0;
    s_QueueLevel++;
}

void QUEUE_DeQueue(void)
{
    if (s_Front == s_Rear)
    {
        s_Front = 0;
        s_Rear = 0;
    }
    else
    {
        s_Front++;
        s_Front = s_Front % QUEUE_MAX_QUEUE;
        s_QueueLevel--;
    }
}

void QUEUE_PushData(const uint8_t data)
{
    s_Queue[s_Rear].data[s_Count] = data;
    s_Count++;
}

QUEUE_struct_t *QUEUE_PopData(void)
{
    return &s_Queue[s_Front];
}