#include <stdio.h>
#include <string.h>

#include "common.h"
#include "order.h"
#include "priority_queue.h"
#include "console_ui.h"


/* =========================================
   PRIORITY QUEUE NODE
   ========================================= */

typedef struct
{
    int orderId;
    int priority;

} PriorityOrder;


/* =========================================
   PRIORITY QUEUE DATA
   ========================================= */

PriorityOrder priorityQueue[MAX_QUEUE];

int priorityQueueSize = 0;


/* =========================================
   INITIALIZE
   ========================================= */

void initializePriorityQueue(void)
{
    priorityQueueSize = 0;
}


/* =========================================
   CHECK EMPTY
   ========================================= */

int isPriorityQueueEmpty(void)
{
    if (priorityQueueSize == 0)
    {
        return 1;
    }

    return 0;
}


/* =========================================
   INSERT
   ========================================= */

void insertPriorityOrder(int orderId, int priority)
{
    int i;
    const char *status = getOrderStatus(orderId);

    if (status == NULL)
    {
        printf("\nOrder not found.\n");
        return;
    }

    if (strcmp(status, "OUT_FOR_DELIVERY") == 0 ||
        strcmp(status, "DELIVERED") == 0 ||
        strcmp(status, "CANCELLED") == 0)
    {
        printf("\nThis order is not waiting for delivery.\n");
        return;
    }

    for (i = 0; i < priorityQueueSize; i++)
    {
        if (priorityQueue[i].orderId == orderId)
        {
            printf("\nOrder %d is already in the priority queue.\n", orderId);
            return;
        }
    }

    if (priorityQueueSize >= MAX_QUEUE)
    {
        printf("\nPriority queue is full!\n");
        return;
    }

    if (priority < 1 || priority > 3)
    {
        printf("\nInvalid priority!\n");
        return;
    }


    /*
       Priority 1 = Highest
       Priority 2 = Medium
       Priority 3 = Normal
    */

    i = priorityQueueSize - 1;


    while (i >= 0 &&
           priorityQueue[i].priority > priority)
    {
        priorityQueue[i + 1] =
            priorityQueue[i];

        i--;
    }


    priorityQueue[i + 1].orderId = orderId;

    priorityQueue[i + 1].priority = priority;

    priorityQueueSize++;


    printf("\nOrder %d added to priority queue.\n",
           orderId);
}

void restorePriorityOrder(int orderId, int priority)
{
    int i;

    if (priorityQueueSize >= MAX_QUEUE || priority < 1 || priority > 3)
    {
        return;
    }

    for (i = 0; i < priorityQueueSize; i++)
    {
        if (priorityQueue[i].orderId == orderId)
        {
            return;
        }
    }

    i = priorityQueueSize - 1;
    while (i >= 0 && priorityQueue[i].priority > priority)
    {
        priorityQueue[i + 1] = priorityQueue[i];
        i--;
    }

    priorityQueue[i + 1].orderId = orderId;
    priorityQueue[i + 1].priority = priority;
    priorityQueueSize++;
}

void removePriorityOrder(int orderId)
{
    int i;

    for (i = 0; i < priorityQueueSize; i++)
    {
        if (priorityQueue[i].orderId == orderId)
        {
            int next;

            for (next = i; next < priorityQueueSize - 1; next++)
            {
                priorityQueue[next] = priorityQueue[next + 1];
            }

            priorityQueueSize--;
            return;
        }
    }
}


/* =========================================
   REMOVE HIGHEST PRIORITY
   ========================================= */

int removeHighestPriorityOrder(void)
{
    int orderId;

    int i;


    if (isPriorityQueueEmpty())
    {
        printf("\nPriority queue is empty!\n");
        return -1;
    }


    orderId = priorityQueue[0].orderId;


    for (i = 0;
         i < priorityQueueSize - 1;
         i++)
    {
        priorityQueue[i] =
            priorityQueue[i + 1];
    }


    priorityQueueSize--;


    printf("\nHighest priority order: %d\n",
           orderId);

    printf("Order removed from priority queue.\n");


    return orderId;
}

int peekHighestPriorityOrder(void)
{
    if (isPriorityQueueEmpty())
    {
        return -1;
    }

    return priorityQueue[0].orderId;
}


/* =========================================
   DISPLAY
   ========================================= */

void displayPriorityQueue(void)
{
    int i;

    if (isPriorityQueueEmpty())
    {
        printf("\nPriority queue is empty.\n");
        return;
    }

    uiHeader("PRIORITY QUEUE");

    for (i = 0; i < priorityQueueSize; i++)
    {
        printf("Order ID: %d | Priority: %d\n",
               priorityQueue[i].orderId,
               priorityQueue[i].priority);
    }

    uiDivider();
}
