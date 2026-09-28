#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"


/* Initialize FIFO queue */
void initializeQueue(void);


/* Add order to queue */
void enqueueOrder(int orderId);


/* Remove first order */
int dequeueOrder(void);

int peekNextOrder(void);

void removeOrderFromQueue(int orderId);


/* Display queue */
void displayQueue(void);


/* Check empty */
int isQueueEmpty(void);


/* Check full */
int isQueueFull(void);

#endif
