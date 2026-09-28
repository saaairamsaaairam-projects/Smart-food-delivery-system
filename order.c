#include <stdio.h>
#include <string.h>

#include "common.h"
#include "input.h"
#include "order.h"
#include "cart.h"
#include "queue.h"
#include "priority_queue.h"
#include "console_ui.h"
Order orders[MAX_ORDERS];

int orderCount = 0;

static int isKnownOrderStatus(const char *status)
{
    return status != NULL &&
           (strcmp(status, "PLACED") == 0 ||
            strcmp(status, "PREPARING") == 0 ||
            strcmp(status, "OUT_FOR_DELIVERY") == 0 ||
            strcmp(status, "DELIVERED") == 0 ||
            strcmp(status, "CANCELLED") == 0);
}


/* ============================================
   SAVE ORDERS
   ============================================ */

int saveOrders(void)
{
    FILE *file;
    int success;

    file = fopen("data/orders.dat", "wb");

    if (file == NULL)
    {
        printf("\nError: Could not save order data!\n");
        return 0;
    }

    success = fwrite(&orderCount, sizeof(int), 1, file) == 1 &&
              fwrite(orders, sizeof(Order), orderCount, file) ==
                  (size_t)orderCount;

    if (fclose(file) != 0)
    {
        success = 0;
    }

    if (!success)
    {
        printf("\nError: Order data could not be fully saved.\n");
    }

    return success;
}


/* ============================================
   LOAD ORDERS
   ============================================ */

void loadOrders(void)
{
    FILE *file;
    int i;

    initializePriorityQueue();

    file = fopen("data/orders.dat", "rb");

    if (file == NULL)
    {
        orderCount = 0;
        return;
    }

    if (fread(&orderCount, sizeof(int), 1, file) != 1)
    {
        orderCount = 0;
        fclose(file);
        printf("\nOrder data is unreadable; starting with no orders.\n");
        return;
    }

    if (orderCount < 0 ||
        orderCount > MAX_ORDERS)
    {
        orderCount = 0;
        fclose(file);
        printf("\nOrder data has an invalid record count.\n");
        return;
    }

    if (fread(orders, sizeof(Order), orderCount, file) != (size_t)orderCount)
    {
        orderCount = 0;
        fclose(file);
        printf("\nOrder data is incomplete; starting with no orders.\n");
        return;
    }

    fclose(file);

    for (i = 0; i < orderCount; i++)
    {
        int itemIndex;

        if (memchr(orders[i].status, '\0', sizeof(orders[i].status)) == NULL)
        {
            orderCount = 0;
            initializePriorityQueue();
            printf("\nOrder data contains an invalid status field.\n");
            return;
        }

        if (strcmp(orders[i].status, "OUT_FOR_DElIVERY") == 0)
        {
            strcpy(orders[i].status, "OUT_FOR_DELIVERY");
        }

        if (orders[i].orderId <= 0 ||
            orders[i].customerId <= 0 ||
            orders[i].restaurantId <= 0 ||
            orders[i].itemCount < 1 ||
            orders[i].itemCount > MAX_ORDER_ITEMS ||
            orders[i].priority < 1 ||
            orders[i].priority > 3 ||
            !isKnownOrderStatus(orders[i].status))
        {
            orderCount = 0;
            initializePriorityQueue();
            printf("\nOrder data contains an invalid record.\n");
            return;
        }

        for (itemIndex = 0; itemIndex < orders[i].itemCount; itemIndex++)
        {
            if (orders[i].items[itemIndex].itemId <= 0 ||
                orders[i].items[itemIndex].quantity <= 0 ||
                orders[i].items[itemIndex].unitPrice <= 0 ||
                memchr(orders[i].items[itemIndex].itemName, '\0',
                       sizeof(orders[i].items[itemIndex].itemName)) == NULL)
            {
                orderCount = 0;
                initializePriorityQueue();
                printf("\nOrder data contains an invalid item.\n");
                return;
            }
        }

        if (strcmp(orders[i].status, "OUT_FOR_DELIVERY") != 0 &&
            strcmp(orders[i].status, "DELIVERED") != 0 &&
            strcmp(orders[i].status, "CANCELLED") != 0)
        {
            restorePriorityOrder(orders[i].orderId, orders[i].priority);
        }
    }
}


/* ============================================
   CREATE ORDER FROM CART
   ============================================ */

void createOrder(int customerId)
{
    int i;
    int priority;

    Order newOrder;

    if (isCartEmpty())
    {
        uiHeader("CART IS EMPTY");

        printf("Please add food items to the cart first.\n");

        return;
    }

    if (orderCount >= MAX_ORDERS)
    {
        printf("\nOrder limit reached!\n");
        return;
    }


    /* ----------------------------------------
       Ask for priority
       ---------------------------------------- */

    uiHeader("CHECKOUT");

    displayCart();

    do
    {
        printf("\nPriority: 1 High, 2 Medium, 3 Low\n");
        if (!readInt("Choice: ", &priority))
        {
            return;
        }

        if (priority < 1 || priority > 3)
        {
            printf("Choose a priority from 1 to 3.\n");
        }
    } while (priority < 1 || priority > 3);


    /* ----------------------------------------
       Create basic order information
       ---------------------------------------- */

    newOrder.orderId = orderCount + 1;

    newOrder.customerId = customerId;

    newOrder.restaurantId = cart[0].restaurantId;

    newOrder.itemCount = 0;

    newOrder.totalAmount = 0;

    newOrder.priority = priority;

    strcpy(newOrder.status, "PLACED");


    /* ----------------------------------------
       Copy all cart items into the order
       ---------------------------------------- */

    for (i = 0; i < getCartItemCount(); i++)
    {
        if (i >= MAX_ORDER_ITEMS)
        {
            break;
        }

        newOrder.items[i].itemId =
            cart[i].itemId;

        strcpy(newOrder.items[i].itemName,
               cart[i].itemName);

        newOrder.items[i].quantity =
            cart[i].quantity;

        newOrder.items[i].unitPrice =
            cart[i].unitPrice;

        newOrder.items[i].itemTotal =
            cart[i].itemTotal;


        newOrder.totalAmount +=
            cart[i].itemTotal;

        newOrder.itemCount++;
    }


    /* ----------------------------------------
       Save order
       ---------------------------------------- */

    orders[orderCount] = newOrder;

    orderCount++;

    if (!saveOrders())
    {
        orderCount--;
        printf("\nOrder was not placed because it could not be saved.\n");
        return;
    }

    insertPriorityOrder(newOrder.orderId, newOrder.priority);
    /* ----------------------------------------
       Display success message
       ---------------------------------------- */

    uiHeader("ORDER PLACED");

    printf("Order ID       : %d\n",
           newOrder.orderId);

    printf("Restaurant ID  : %d\n",
           newOrder.restaurantId);

    printf("Number of Items: %d\n",
           newOrder.itemCount);

    printf("Total Amount   : Rs. %.2f\n",
           newOrder.totalAmount);

    printf("Priority       : %d\n",
           newOrder.priority);

    printf("Status         : %s\n",
           newOrder.status);

    uiDivider();


    /* ----------------------------------------
       Clear cart after successful checkout
       ---------------------------------------- */

    clearCart();
}


/* ============================================
   DISPLAY ALL ORDERS
   ============================================ */

void displayOrders(void)
{
    int i;
    int j;

    if (orderCount == 0)
    {
        printf("\nNo orders available.\n");
        return;
    }

    uiHeader("ALL ORDERS");

    for (i = 0; i < orderCount; i++)
    {
        uiDivider();

        printf("Order ID      : %d\n",
               orders[i].orderId);

        printf("Customer ID   : %d\n",
               orders[i].customerId);

        printf("Restaurant ID : %d\n",
               orders[i].restaurantId);

        printf("Priority      : %d\n",
               orders[i].priority);

        printf("Status        : %s\n",
               orders[i].status);

        printf("Items         :\n");

        for (j = 0; j < orders[i].itemCount; j++)
        {
            printf("  %d. %s\n",
                   j + 1,
                   orders[i].items[j].itemName);

            printf("     Quantity : %d\n",
                   orders[i].items[j].quantity);

            printf("     Price    : Rs. %.2f\n",
                   orders[i].items[j].unitPrice);

            printf("     Subtotal : Rs. %.2f\n",
                   orders[i].items[j].itemTotal);
        }

        printf("Total Amount  : Rs. %.2f\n",
               orders[i].totalAmount);
    }

    uiDivider();
}


/* ============================================
   FIND ORDER BY ID
   ============================================ */

int findOrderById(int orderId)
{
    int i;

    for (i = 0; i < orderCount; i++)
    {
        if (orders[i].orderId == orderId)
        {
            return i;
        }
    }

    return -1;
}


/* ============================================
   DISPLAY ONE ORDER
   ============================================ */

void displayOrderById(int orderId)
{
    int index;
    int i;

    index = findOrderById(orderId);

    if (index == -1)
    {
        printf("\nOrder not found!\n");
        return;
    }

    uiHeader("ORDER DETAILS");

    printf("Order ID      : %d\n",
           orders[index].orderId);

    printf("Customer ID   : %d\n",
           orders[index].customerId);

    printf("Restaurant ID : %d\n",
           orders[index].restaurantId);

    printf("Priority      : %d\n",
           orders[index].priority);

    printf("Status        : %s\n",
           orders[index].status);

    printf("\nItems:\n");

    for (i = 0; i < orders[index].itemCount; i++)
    {
        printf("\n%d. %s\n",
               i + 1,
               orders[index].items[i].itemName);

        printf("   Quantity : %d\n",
               orders[index].items[i].quantity);

        printf("   Price    : Rs. %.2f\n",
               orders[index].items[i].unitPrice);

        printf("   Subtotal : Rs. %.2f\n",
               orders[index].items[i].itemTotal);
    }

    printf("\n--------------------------------------------\n");

    printf("TOTAL : Rs. %.2f\n",
           orders[index].totalAmount);

    uiDivider();
}


/* ============================================
   UPDATE ORDER STATUS
   ============================================ */

int updateOrderStatus(int orderId,
                      const char *status)
{
    int index;
    char previousStatus[30];

    index = findOrderById(orderId);

    if (index == -1)
    {
        printf("\nOrder not found!\n");
        return 0;
    }

    if (!isKnownOrderStatus(status))
    {
        printf("\nInvalid order status.\n");
        return 0;
    }

    if ((strcmp(orders[index].status, "DELIVERED") == 0 ||
         strcmp(orders[index].status, "CANCELLED") == 0) &&
        strcmp(orders[index].status, status) != 0)
    {
        printf("\nA completed or cancelled order cannot be reopened.\n");
        return 0;
    }

    if (strcmp(orders[index].status, status) != 0)
    {
        int validTransition =
            (strcmp(orders[index].status, "PLACED") == 0 &&
             (strcmp(status, "PREPARING") == 0 ||
              strcmp(status, "OUT_FOR_DELIVERY") == 0 ||
              strcmp(status, "CANCELLED") == 0)) ||
            (strcmp(orders[index].status, "PREPARING") == 0 &&
             (strcmp(status, "OUT_FOR_DELIVERY") == 0 ||
              strcmp(status, "CANCELLED") == 0)) ||
            (strcmp(orders[index].status, "OUT_FOR_DELIVERY") == 0 &&
             strcmp(status, "DELIVERED") == 0);

        if (!validTransition)
        {
            printf("\nThat order status transition is not allowed.\n");
            return 0;
        }
    }

    strcpy(previousStatus, orders[index].status);
    strcpy(orders[index].status, status);

    if (!saveOrders())
    {
        strcpy(orders[index].status, previousStatus);
        return 0;
    }

    if (strcmp(status, "OUT_FOR_DELIVERY") == 0 ||
        strcmp(status, "DELIVERED") == 0 ||
        strcmp(status, "CANCELLED") == 0)
    {
        removeOrderFromQueue(orderId);
        removePriorityOrder(orderId);
    }

    printf("\nOrder status updated successfully!\n");

    printf("Order ID : %d\n",
           orders[index].orderId);

    printf("Status   : %s\n",
           orders[index].status);

    return 1;
}


/* ============================================
   GET ORDER COUNT
   ============================================ */

int getOrderCount(void)
{
    return orderCount;
}


void displayOrderReport(void)
{
    int i;
    int placed = 0;
    int preparing = 0;
    int outForDelivery = 0;
    int delivered = 0;
    int cancelled = 0;
    int highPriority = 0;
    int mediumPriority = 0;
    int lowPriority = 0;
    int deliveredItems = 0;
    float completedRevenue = 0;

    for (i = 0; i < orderCount; i++)
    {
        if (strcmp(orders[i].status, "PLACED") == 0)
        {
            placed++;
        }
        else if (strcmp(orders[i].status, "PREPARING") == 0)
        {
            preparing++;
        }
        else if (strcmp(orders[i].status, "OUT_FOR_DELIVERY") == 0)
        {
            outForDelivery++;
        }
        else if (strcmp(orders[i].status, "DELIVERED") == 0)
        {
            int itemIndex;

            delivered++;
            completedRevenue += orders[i].totalAmount;
            for (itemIndex = 0; itemIndex < orders[i].itemCount; itemIndex++)
            {
                deliveredItems += orders[i].items[itemIndex].quantity;
            }
        }
        else if (strcmp(orders[i].status, "CANCELLED") == 0)
        {
            cancelled++;
        }

        if (orders[i].priority == 1)
        {
            highPriority++;
        }
        else if (orders[i].priority == 2)
        {
            mediumPriority++;
        }
        else if (orders[i].priority == 3)
        {
            lowPriority++;
        }
    }

    uiHeader("OPERATIONS REPORT");
    printf("Total orders       : %d\n", orderCount);
    printf("Pending            : %d\n", placed + preparing + outForDelivery);
    printf("  Placed           : %d\n", placed);
    printf("  Preparing        : %d\n", preparing);
    printf("  Out for delivery : %d\n", outForDelivery);
    printf("Delivered          : %d\n", delivered);
    printf("Cancelled          : %d\n", cancelled);
    printf("\nPriority mix\n");
    printf("  High              : %d\n", highPriority);
    printf("  Medium            : %d\n", mediumPriority);
    printf("  Low               : %d\n", lowPriority);
    printf("\nCompleted revenue  : Rs. %.2f\n", completedRevenue);
    printf("Items delivered    : %d\n", deliveredItems);
    uiDivider();
}


/* ============================================
   DISPLAY CUSTOMER ORDERS
   ============================================ */

void displayOrdersByCustomerId(int customerId)
{
    int i;
    int j;
    int found = 0;

    uiHeader("MY ORDERS");

    for (i = 0; i < orderCount; i++)
    {
        if (orders[i].customerId == customerId)
        {
            found = 1;

            uiDivider();

            printf("Order ID      : %d\n",
                   orders[i].orderId);

            printf("Restaurant ID : %d\n",
                   orders[i].restaurantId);

            printf("Priority      : %d\n",
                   orders[i].priority);

            printf("Status        : %s\n",
                   orders[i].status);

            printf("Items:\n");

            for (j = 0; j < orders[i].itemCount; j++)
            {
                printf("  %s x%d\n",
                       orders[i].items[j].itemName,
                       orders[i].items[j].quantity);
            }

            printf("Total         : Rs. %.2f\n",
                   orders[i].totalAmount);
        }
    }

    if (!found)
    {
        printf("\nYou have no orders.\n");
    }

    uiDivider();
}


void trackCustomerOrder(int customerId)
{
    static const char *stages[] =
    {
        "Order placed",
        "Preparing",
        "Out for delivery",
        "Delivered"
    };
    int orderId;
    int index;
    int currentStage;
    int i;

    if (!readInt("Order ID to track: ", &orderId))
    {
        return;
    }

    index = findOrderById(orderId);
    if (index == -1 || orders[index].customerId != customerId)
    {
        printf("\nThat order was not found in your account.\n");
        return;
    }

    uiHeader("ORDER TRACKING");
    printf("Order ID : %d\n", orders[index].orderId);
    printf("Status   : %s\n\n", orders[index].status);

    if (strcmp(orders[index].status, "CANCELLED") == 0)
    {
        printf("[!] Order cancelled\n");
        return;
    }

    if (strcmp(orders[index].status, "PLACED") == 0)
    {
        currentStage = 0;
    }
    else if (strcmp(orders[index].status, "PREPARING") == 0)
    {
        currentStage = 1;
    }
    else if (strcmp(orders[index].status, "OUT_FOR_DELIVERY") == 0)
    {
        currentStage = 2;
    }
    else
    {
        currentStage = 3;
    }

    for (i = 0; i < 4; i++)
    {
        int state = i < currentStage ? 2 : (i == currentStage ? 1 : 0);
        uiProgressStep(state, stages[i]);
    }
}


/* ============================================
   CANCEL ORDER
   ============================================ */

int cancelOrderForCustomer(int orderId,
                           int customerId)
{
    int index;

    index = findOrderById(orderId);

    if (index == -1)
    {
        printf("\nOrder not found!\n");
        return 0;
    }

    if (orders[index].customerId != customerId)
    {
        printf("\nYou cannot cancel another customer's order!\n");
        return 0;
    }

    if (strcmp(orders[index].status, "DELIVERED") == 0)
    {
        printf("\nDelivered orders cannot be cancelled!\n");
        return 0;
    }

    if (strcmp(orders[index].status, "CANCELLED") == 0)
    {
        printf("\nOrder is already cancelled!\n");
        return 0;
    }

    if (strcmp(orders[index].status, "OUT_FOR_DELIVERY") == 0)
    {
        printf("\nAn order already out for delivery cannot be cancelled.\n");
        return 0;
    }

    return updateOrderStatus(orderId, "CANCELLED");
}


/* ============================================
   GET ORDER PRIORITY
   ============================================ */

int getOrderPriority(int orderId)
{
    int index;

    index = findOrderById(orderId);

    if (index == -1)
    {
        return -1;
    }

    return orders[index].priority;
}

const char *getOrderStatus(int orderId)
{
    int index = findOrderById(orderId);

    if (index == -1)
    {
        return NULL;
    }

    return orders[index].status;
}