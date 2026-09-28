#ifndef MENU_H
#define MENU_H

#define MAX_MENU_ITEMS 500

typedef struct
{
    int itemId;
    int restaurantId;
    char itemName[100];
    float price;
} MenuItem;

void displayMenu(int restaurantId);

void loadMenuItems(void);

void addMenuItem(void);

int hasMenu(int restaurantId);

int getMenuItem(int restaurantId,
                int itemId,
                char *itemName,
                float *price);

#endif