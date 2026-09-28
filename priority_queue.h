#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "common.h"


/* Initialize priority queue */
void initializePriorityQueue(void);


/* Insert order */
void insertPriorityOrder(int orderId, int priority);

void restorePriorityOrder(int orderId, int priority);


/* Remove highest priority order */
int removeHighestPriorityOrder(void);

int peekHighestPriorityOrder(void);

void removePriorityOrder(int orderId);


/* Display priority queue */
void displayPriorityQueue(void);


/* Check empty */
int isPriorityQueueEmpty(void);

#endif