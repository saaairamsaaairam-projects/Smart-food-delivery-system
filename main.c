#include <stdio.h>

#include "auth.h"
#include "common.h"
#include "order.h"
#include "queue.h"
#include "priority_queue.h"
#include "cart.h"
#include "menu.h"
#include "graph.h"
#include "input.h"
#include "console_ui.h"


int findRestaurantById(int restaurantId);
void addCustomer(void);
void displayCustomers(void);
void loadCustomers(void);

void addRestaurant(void);
void displayRestaurants(void);
void searchRestaurant(void);
void loadRestaurants(void);


/* Admin Dashboard */
void adminMenu(void)
{
    int choice;
    int orderId;
    int priority;

    while (1)
    {
        char summary[64];

        snprintf(summary, sizeof(summary), "%d orders in system", getOrderCount());
        uiDashboardHeader("ADMIN", summary);
        uiGroup("RESTAURANTS & CUSTOMERS");
        uiOption(1, "Add restaurant");
        uiOption(2, "View restaurants");
        uiOption(3, "Search restaurants");
        uiOption(4, "View customers");

        uiGroup("ORDER OPERATIONS");
        uiOption(5, "View all orders");
        uiOption(6, "Update order status");
        uiOption(7, "Add order to FIFO queue");
        uiOption(8, "View FIFO queue");
        uiOption(9, "Dispatch next FIFO order");
        uiOption(10, "Add order to priority queue");
        uiOption(11, "View priority queue");
        uiOption(12, "Dispatch highest-priority order");

        uiGroup("DELIVERY & REPORTS");
        uiOption(13, "View delivery graph");
        uiOption(14, "Find shortest delivery route");
        uiOption(15, "Add a menu item");
        uiOption(16, "View operations report");
        uiOption(0, "Log out");

        if (!readInt("Choice: ", &choice))
        {
            return;
        }

        switch (choice)
        {
            case 1:
                addRestaurant();
                break;

            case 2:
                displayRestaurants();
                break;

            case 3:
                searchRestaurant();
                break;

            case 4:
                displayCustomers();
                break;

            case 5:
                displayOrders();
                break;

            case 6:
                {
                    static const char *statuses[] =
                    {
                        "PLACED",
                        "PREPARING",
                        "OUT_FOR_DELIVERY",
                        "DELIVERED",
                        "CANCELLED"
                    };
                    int statusChoice;

                    if (!readInt("Order ID: ", &orderId))
                    {
                        return;
                    }

                    uiOption(1, "Placed");
                    uiOption(2, "Preparing");
                    uiOption(3, "Out for delivery");
                    uiOption(4, "Delivered");
                    uiOption(5, "Cancelled");

                    if (!readInt("New status: ", &statusChoice))
                    {
                        return;
                    }

                    if (statusChoice < 1 || statusChoice > 5)
                    {
                        printf("Invalid status choice.\n");
                        break;
                    }

                    updateOrderStatus(orderId, statuses[statusChoice - 1]);
                }

                break;

            case 7:
                if (!readInt("Order ID: ", &orderId))
                {
                    return;
                }
                enqueueOrder(orderId);
                break;

            case 8:
                displayQueue();
                break;

            case 9:
            {
                int dispatchedOrderId;
                dispatchedOrderId = peekNextOrder();
                if (dispatchedOrderId != -1)
                {
                    updateOrderStatus(dispatchedOrderId, "OUT_FOR_DELIVERY");
                }
                else
                {
                    printf("\nFIFO queue is empty.\n");
                }
                break;
            }
            case 10:
                {
                    if (!readInt("Order ID: ", &orderId))
                    {
                        return;
                    }

                    priority = getOrderPriority(orderId);

                    if (priority == -1)
                    {
                        printf("\nOrder not found!\n");
                    }
                    else
                    {
                        insertPriorityOrder(orderId, priority);
                    }

                    break;
                }

            case 11:
                displayPriorityQueue();
                break;

            case 12:
                {
                    int deliveredOrderId;

                    deliveredOrderId = peekHighestPriorityOrder();

                    if (deliveredOrderId != -1)
                    {
                        updateOrderStatus(deliveredOrderId, "OUT_FOR_DELIVERY");
                    }
                    else
                    {
                        printf("\nPriority queue is empty.\n");
                    }

                    break;
                }
            case 13:
                displayGraph();
                break;
            case 14:
                {
                    int source;
                    int destination;

                    uiHeader("ROUTE LOCATIONS");
                    uiOption(0, "Restaurant");
                    uiOption(1, "MVP Colony");
                    uiOption(2, "Dwaraka Nagar");
                    uiOption(3, "RTC Complex");
                    uiOption(4, "Customer");

                    if (!readInt("Start location ID: ", &source) ||
                        !readInt("Destination location ID: ", &destination))
                    {
                        return;
                    }

                    displayShortestPath(source, destination);
                    break;
                }
            case 15:
                addMenuItem();
                break;
            case 16:
                displayOrderReport();
                break;
            case 0:
                printf("\nAdmin logged out successfully.\n");
                return;

            default:
                printf("\nInvalid choice!\n");
        }
    }
}


void customerMenu(int customerId)
{
    int choice;
    int orderId;

    while (1)
    {
        char summary[100];

        snprintf(summary, sizeof(summary), "Account #%d  |  Cart: %d items  |  Rs. %.2f",
                 customerId, getCartItemCount(), getCartTotal());
        uiDashboardHeader("CUSTOMER", summary);
        uiGroup("DISCOVER & ORDER");
        uiOption(1, "View restaurants");
        uiOption(2, "Search restaurants");
        uiOption(3, "Create an order");

        uiGroup("YOUR ACTIVITY");
        uiOption(4, "View my orders");
        uiOption(5, "View or clear cart");
        uiOption(6, "Cancel an order");
        uiOption(7, "Track an order");
        uiOption(0, "Log out");

        if (!readInt("Choice: ", &choice))
        {
            return;
        }

        switch (choice)
        {
            case 1:
                displayRestaurants();
                break;

            case 2:
                searchRestaurant();
                break;

            case 3:
                {
                    int restaurantId;
                    int itemId;
                    int quantity;
                    int anotherItem;
                    int checkoutChoice;

                    char itemName[100];
                    float unitPrice;

                    displayRestaurants();

                    if (!readInt("Restaurant ID: ", &restaurantId))
                    {
                        return;
                    }

                    if (findRestaurantById(restaurantId) == -1)
                    {
                        printf("\nInvalid Restaurant ID!\n");
                        break;
                    }

                    if (!hasMenu(restaurantId))
                    {
                        printf("\nNo menu is configured for this restaurant yet.\n");
                        printf("Ask an administrator to add menu items first.\n");
                        break;
                    }

                    do
                    {
                        uiHeader("RESTAURANT MENU");
                        displayMenu(restaurantId);

                        if (!readInt("Food item ID: ", &itemId))
                        {
                            return;
                        }

                        if (!getMenuItem(restaurantId,
                                        itemId,
                                        itemName,
                                        &unitPrice))
                        {
                            printf("\nInvalid Food Item ID!\n");
                            continue;
                        }

                        printf("\nSelected Item : %s\n", itemName);
                        printf("Price         : Rs. %.2f\n", unitPrice);

                        if (!readInt("Quantity: ", &quantity))
                        {
                            return;
                        }

                        if (quantity <= 0 || quantity > MAX_ITEM_QUANTITY)
                        {
                            printf("\nQuantity must be between 1 and %d.\n",
                                   MAX_ITEM_QUANTITY);
                            continue;
                        }

                        addToCart(restaurantId,
                                itemId,
                                itemName,
                                unitPrice,
                                quantity);

                        displayCart();

                        do
                        {
                            if (!readInt("Add another item? (1 Yes, 2 No): ",
                                         &anotherItem))
                            {
                                return;
                            }

                            if (anotherItem != 1 && anotherItem != 2)
                            {
                                printf("Choose 1 or 2.\n");
                            }
                        } while (anotherItem != 1 && anotherItem != 2);

                    } while (anotherItem == 1);

                     displayCart();

                    if (!isCartEmpty())
                    {
                        do
                        {
                            if (!readInt("1 Checkout, 2 Return to menu: ",
                                         &checkoutChoice))
                            {
                                return;
                            }

                            if (checkoutChoice != 1 && checkoutChoice != 2)
                            {
                                printf("Choose 1 or 2.\n");
                            }
                        } while (checkoutChoice != 1 && checkoutChoice != 2);

                        if (checkoutChoice == 1)
                        {
                            checkoutCart(customerId);
                        }
                        else
                        {
                            printf("Returning to customer menu.\n");
                        }
                    }

                    break;
                }

            case 4:
                displayOrdersByCustomerId(customerId);
                break;

            case 5:
                displayCart();
                if (!isCartEmpty())
                {
                    int clearChoice;

                    do
                    {
                        if (!readInt("1 Keep cart, 2 Clear cart: ", &clearChoice))
                        {
                            return;
                        }

                        if (clearChoice != 1 && clearChoice != 2)
                        {
                            printf("Choose 1 or 2.\n");
                        }
                    } while (clearChoice != 1 && clearChoice != 2);

                    if (clearChoice == 2)
                    {
                        clearCart();
                    }
                }
                break;

            case 6:
                if (!readInt("Order ID to cancel: ", &orderId))
                {
                    return;
                }
                cancelOrderForCustomer(orderId, customerId);
                break;

            case 7:
                trackCustomerOrder(customerId);
                break;

            case 0:
                printf("\nCustomer logged out successfully.\n");
                return;

            default:
                printf("\nInvalid choice!\n");
        }
    }
}

int main(void)
{
    int choice;

    uiInitialize();
    initializeQueue();
    initializePriorityQueue();
    initializeGraph();
    loadRestaurants();
    loadMenuItems();
    loadOrders();

    addLocation(0, "Restaurant");
    addLocation(1, "MVP Colony");
    addLocation(2, "Dwaraka Nagar");
    addLocation(3, "RTC Complex");
    addLocation(4, "Customer");

    addRoad(0, 1, 5);
    addRoad(1, 2, 3);
    addRoad(2, 3, 2);
    addRoad(3, 4, 4);
    addRoad(1, 3, 6);

    loadCustomers();

    while (1)
    {
        uiWelcome();
        uiGroup("SELECT A PORTAL");
        uiOption(1, "Admin login");
        uiOption(2, "Customer portal");
        uiOption(0, "Exit");

        if (!readInt("Choice: ", &choice))
        {
            return 0;
        }

        switch (choice)
        {
            case 1:
                adminLogin();
                break;

            case 2:
                customerLogin();
                break;

            case 0:
                uiHeader("THANK YOU FOR USING SMART FOOD DELIVERY");

                return 0;

            default:
                printf("\nInvalid choice!\n");
        }
    }

    return 0;
}
