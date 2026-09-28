#ifndef ORDER_H
#define ORDER_H

#include "common.h"

void createOrder(int customerId);

void displayOrders(void);

int findOrderById(int orderId);

void displayOrderById(int orderId);

int updateOrderStatus(int orderId, const char *status);

int getOrderCount(void);

int saveOrders(void);

void loadOrders(void);

void displayOrdersByCustomerId(int customerId);

void trackCustomerOrder(int customerId);

void displayOrderReport(void);

int cancelOrderForCustomer(int orderId, int customerId);

int getOrderPriority(int orderId);

const char *getOrderStatus(int orderId);

#endif