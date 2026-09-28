#include <stdio.h>
#include <string.h>

#include "cart.h"
#include "menu.h"

#include "auth.h"
#include "common.h"
#include "input.h"
#include "console_ui.h"

void displayOrdersByCustomerId(int customerId);
int cancelOrderForCustomer(int orderId, int customerId);

int registerCustomer(void);
int loginCustomer(void);

void displayRestaurants(void);
void searchRestaurant(void);

void adminMenu(void);
void customerMenu(int customerId);


void adminLogin(void)
{
    char username[50];
    char password[50];

    uiHeader("ADMIN LOGIN");

    if (!readLine("Username: ", username, sizeof(username)) ||
        !readLine("Password: ", password, sizeof(password)))
    {
        return;
    }

    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "admin123") == 0)
    {
        printf("\nLogin successful!\n");
        printf("Welcome, Admin!\n");

        adminMenu();
    }
    else
    {
        printf("\nInvalid username or password!\n");
    }
}


void customerLogin(void)
{
    int choice;
    int customerId;

    while (1)
    {
        uiHeader("CUSTOMER PORTAL");
        uiOption(1, "Create an account");
        uiOption(2, "Log in");
        uiOption(0, "Back");
        uiDivider();

        if (!readInt("Choice: ", &choice))
        {
            return;
        }

        switch (choice)
        {
            case 1:

                registerCustomer();

                break;

            case 2:

                customerId = loginCustomer();

                if (customerId != -1)
                {
                    customerMenu(customerId);
                }

                break;

            case 0:

                printf("\nReturning to main menu...\n");

                return;

            default:

                printf("\nInvalid choice!\n");
        }
    }
}