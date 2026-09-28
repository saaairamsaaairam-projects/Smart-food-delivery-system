#include <stdio.h>
#include <string.h>

#include "common.h"
#include "order.h"
#include "queue.h"
#include "console_ui.h"


/* =========================================
   FIFO QUEUE
   ========================================= */

int deliveryQueue[MAX_QUEUE];

int front = -1;

int rear = -1;


/* =========================================
   INITIALIZE
   ========================================= */

void initializeQueue(void)
{
    front = -1;
    rear = -1;
}


/* =========================================
   CHECK EMPTY
   ========================================= */

int isQueueEmpty(void)
{
    if (front == -1)
    {
        return 1;
    }

    return 0;
}


/* =========================================
   CHECK FULL
   ========================================= */

int isQueueFull(void)
{
    if (rear == MAX_QUEUE - 1)
    {
        return 1;
    }

    return 0;
}


/* =========================================
   ENQUEUE
   ========================================= */

void enqueueOrder(int orderId)
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

    for (i = front; !isQueueEmpty() && i <= rear; i++)
    {
        if (deliveryQueue[i] == orderId)
        {
            printf("\nOrder %d is already in the FIFO queue.\n", orderId);
            return;
        }
    }

    if (isQueueFull())
    {
        printf("\nFIFO queue is full!\n");
        return;
    }

    if (front == -1)
    {
        front = 0;
    }

    rear++;

    deliveryQueue[rear] = orderId;

    printf("\nOrder %d added to FIFO queue.\n",
           orderId);
}

void removeOrderFromQueue(int orderId)
{
    int i;

    if (isQueueEmpty())
    {
        return;
    }

    for (i = front; i <= rear; i++)
    {
        if (deliveryQueue[i] == orderId)
        {
            int next;

            for (next = i; next < rear; next++)
            {
                deliveryQueue[next] = deliveryQueue[next + 1];
            }

            rear--;
            if (front > rear)
            {
                front = -1;
                rear = -1;
            }
            return;
        }
    }
}


/* =========================================
   DEQUEUE
   ========================================= */

int dequeueOrder(void)
{
    int orderId;

    if (isQueueEmpty())
    {
        printf("\nFIFO queue is empty!\n");
        return -1;
    }

    orderId = deliveryQueue[front];

    front++;

    if (front > rear)
    {
        front = -1;
        rear = -1;
    }

    printf("\nOrder %d removed from FIFO queue.\n",
           orderId);

    return orderId;
}

int peekNextOrder(void)
{
    if (isQueueEmpty())
    {
        return -1;
    }

    return deliveryQueue[front];
}


/* =========================================
   DISPLAY QUEUE
   ========================================= */

void displayQueue(void)
{
    int i;

    if (isQueueEmpty())
    {
        printf("\nFIFO queue is empty.\n");
        return;
    }

    uiHeader("FIFO DELIVERY QUEUE");

    for (i = front; i <= rear; i++)
    {
        printf("Order ID: %d\n",
               deliveryQueue[i]);
    }

    uiDivider();
}
