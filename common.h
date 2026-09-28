#ifndef COMMON_H
#define COMMON_H

/* =========================================
   CONSTANTS
   ========================================= */

#define MAX_CUSTOMERS 100
#define MAX_RESTAURANTS 100
#define MAX_ORDERS 100
#define MAX_QUEUE 100
#define MAX_ORDER_ITEMS 20


/* =========================================
   CUSTOMER STRUCTURE
   ========================================= */
typedef struct
{
    int customerId;
    char name[100];
    char phone[20];
    char address[200];
    char username[50];
    char password[50];
} Customer;


/* =========================================
   RESTAURANT STRUCTURE
   ========================================= */

typedef struct
{
    int restaurantId;
    char name[100];
    char cuisine[100];
    char location[150];
    char phone[20];
    float rating;

} Restaurant;


/* =========================================
   ORDER STRUCTURE
   ========================================= */

typedef struct
{
    int itemId;
    char itemName[100];
    int quantity;
    float unitPrice;
    float itemTotal;

} OrderItem;


typedef struct
{
    int orderId;
    int customerId;
    int restaurantId;

    OrderItem items[MAX_ORDER_ITEMS];

    int itemCount;

    float totalAmount;

    char status[30];

    int priority;

} Order;

#endif
