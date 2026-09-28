#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "input.h"
#include "menu.h"
#include "console_ui.h"

int findRestaurantById(int restaurantId);

MenuItem menuItems[MAX_MENU_ITEMS] =
{
    /* Restaurant 1 - Biryani House */
    {1, 1, "Chicken Biryani", 180.00},
    {2, 1, "Mutton Biryani", 250.00},
    {3, 1, "Veg Biryani", 140.00},
    {4, 1, "Chicken 65", 160.00},
    {5, 1, "Paneer Biryani", 170.00},

    /* Restaurant 2 - Pizza Hub */
    {6, 2, "Margherita Pizza", 200.00},
    {7, 2, "Farmhouse Pizza", 250.00},
    {8, 2, "Paneer Pizza", 230.00},
    {9, 2, "Chicken Pizza", 280.00},
    {10, 2, "Garlic Bread", 120.00},

    /* Restaurant 3 - South Indian Meals */
    {11, 3, "Idly", 60.00},
    {12, 3, "Masala Dosa", 90.00},
    {13, 3, "Vada", 50.00},
    {14, 3, "South Indian Meals", 150.00},
    {15, 3, "Paneer Dosa", 120.00}
};

int menuItemCount = 15;


static int saveMenuItems(void)
{
    FILE *file = fopen("data/menus.dat", "wb");
    int success;

    if (file == NULL)
    {
        printf("\nError: Could not save menu data.\n");
        return 0;
    }

    success = fwrite(&menuItemCount, sizeof(int), 1, file) == 1 &&
              fwrite(menuItems, sizeof(MenuItem), menuItemCount, file) ==
                  (size_t)menuItemCount;

    if (fclose(file) != 0)
    {
        success = 0;
    }

    if (!success)
    {
        printf("\nError: Menu data could not be fully saved.\n");
    }

    return success;
}


void loadMenuItems(void)
{
    FILE *file = fopen("data/menus.dat", "rb");
    MenuItem loadedItems[MAX_MENU_ITEMS];
    int loadedCount;
    int i;

    if (file == NULL)
    {
        return;
    }

    if (fread(&loadedCount, sizeof(int), 1, file) != 1 ||
        loadedCount < 15 || loadedCount > MAX_MENU_ITEMS ||
        fread(loadedItems, sizeof(MenuItem), loadedCount, file) !=
            (size_t)loadedCount)
    {
        fclose(file);
        printf("\nMenu data is invalid; using the built-in menus.\n");
        return;
    }

    fclose(file);

    for (i = 0; i < loadedCount; i++)
    {
        if (loadedItems[i].itemId <= 0 ||
            loadedItems[i].restaurantId <= 0 ||
            !isfinite(loadedItems[i].price) ||
            loadedItems[i].price <= 0 || loadedItems[i].price > 1000000.0f ||
            loadedItems[i].itemName[0] == '\0' ||
            memchr(loadedItems[i].itemName, '\0',
                   sizeof(loadedItems[i].itemName)) == NULL)
        {
            printf("\nMenu data contains an invalid item; using built-in menus.\n");
            return;
        }

        for (int previous = 0; previous < i; previous++)
        {
            if (loadedItems[previous].itemId == loadedItems[i].itemId)
            {
                printf("\nMenu data contains duplicate item IDs; using built-in menus.\n");
                return;
            }
        }
    }

    memcpy(menuItems, loadedItems, (size_t)loadedCount * sizeof(MenuItem));
    menuItemCount = loadedCount;
}


int hasMenu(int restaurantId)
{
    int i;

    for (i = 0; i < menuItemCount; i++)
    {
        if (menuItems[i].restaurantId == restaurantId)
        {
            return 1;
        }
    }

    return 0;
}


void addMenuItem(void)
{
    MenuItem newItem;
    int i;
    int highestItemId = 0;

    if (menuItemCount >= MAX_MENU_ITEMS)
    {
        printf("\nMenu item limit reached.\n");
        return;
    }

    uiHeader("ADD MENU ITEM");

    if (!readInt("Restaurant ID: ", &newItem.restaurantId))
    {
        return;
    }

    if (findRestaurantById(newItem.restaurantId) == -1)
    {
        printf("\nRestaurant not found.\n");
        return;
    }

    if (!readLine("Item name: ", newItem.itemName, sizeof(newItem.itemName)) ||
        !readFloat("Price: ", &newItem.price))
    {
        return;
    }

    if (newItem.price <= 0 || newItem.price > 1000000.0f)
    {
        printf("\nPrice must be greater than zero and no more than 1,000,000.\n");
        return;
    }

    for (i = 0; i < menuItemCount; i++)
    {
        if (menuItems[i].itemId > highestItemId)
        {
            highestItemId = menuItems[i].itemId;
        }
    }

    newItem.itemId = highestItemId + 1;
    menuItems[menuItemCount++] = newItem;

    if (!saveMenuItems())
    {
        menuItemCount--;
        printf("\nMenu item was not added because it could not be saved.\n");
        return;
    }

    printf("\nMenu item added. Item ID: %d\n", newItem.itemId);
}


/* ============================================
   DISPLAY RESTAURANT MENU
   ============================================ */

void displayMenu(int restaurantId)
{
    int i;
    int found = 0;

    uiHeader("RESTAURANT MENU");
    printf("%-5s %-28s %12s\n", "ID", "ITEM", "PRICE");
    uiDivider();

    for (i = 0; i < menuItemCount; i++)
    {
        if (menuItems[i].restaurantId == restaurantId)
        {
                 printf("%-5d %-28.28s Rs. %8.2f\n",
                   menuItems[i].itemId,
                   menuItems[i].itemName,
                   menuItems[i].price);

            found = 1;
        }
    }

    if (!found)
    {
        printf("\nNo menu available for this restaurant.\n");
    }

    uiDivider();
}


/* ============================================
   GET SELECTED MENU ITEM
   ============================================ */

int getMenuItem(int restaurantId,
                int itemId,
                char *itemName,
                float *price)
{
    int i;

    for (i = 0; i < menuItemCount; i++)
    {
        if (menuItems[i].restaurantId == restaurantId &&
            menuItems[i].itemId == itemId)
        {
            strcpy(itemName, menuItems[i].itemName);
            *price = menuItems[i].price;

            return 1;
        }
    }

    return 0;
}