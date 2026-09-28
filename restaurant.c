
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "input.h"
#include "console_ui.h"

Restaurant restaurants[MAX_RESTAURANTS];

int restaurantCount = 0;

static int containsIgnoreCase(const char *text, const char *query)
{
    const char *start;

    for (start = text; *start != '\0'; start++)
    {
        const char *textCharacter = start;
        const char *queryCharacter = query;

        while (*textCharacter != '\0' && *queryCharacter != '\0' &&
               tolower((unsigned char)*textCharacter) ==
                   tolower((unsigned char)*queryCharacter))
        {
            textCharacter++;
            queryCharacter++;
        }

        if (*queryCharacter == '\0')
        {
            return 1;
        }
    }

    return *query == '\0';
}


/* ============================================
   SAVE RESTAURANTS
   ============================================ */

int saveRestaurants(void)
{
    FILE *file;
    int success;

    file = fopen("data/restaurants.dat", "wb");

    if (file == NULL)
    {
        printf("\nError: Could not save restaurant data!\n");
        return 0;
    }

    success = fwrite(&restaurantCount, sizeof(int), 1, file) == 1 &&
              fwrite(restaurants, sizeof(Restaurant), restaurantCount, file) ==
                  (size_t)restaurantCount;

    if (fclose(file) != 0)
    {
        success = 0;
    }

    if (!success)
    {
        printf("\nError: Restaurant data could not be fully saved.\n");
    }

    return success;
}


/* ============================================
   LOAD RESTAURANTS
   ============================================ */

void loadRestaurants(void)
{
    FILE *file;
    int i;

    file = fopen("data/restaurants.dat", "rb");

    if (file == NULL)
    {
        restaurantCount = 0;
        return;
    }

    if (fread(&restaurantCount, sizeof(int), 1, file) != 1)
    {
        restaurantCount = 0;
        fclose(file);
        printf("\nRestaurant data is unreadable; starting with no restaurants.\n");
        return;
    }

    if (restaurantCount < 0 || restaurantCount > MAX_RESTAURANTS)
    {
        restaurantCount = 0;
        fclose(file);
        printf("\nRestaurant data has an invalid record count.\n");
        return;
    }

    if (fread(restaurants, sizeof(Restaurant), restaurantCount, file) !=
        (size_t)restaurantCount)
    {
        restaurantCount = 0;
        fclose(file);
        printf("\nRestaurant data is incomplete; starting with no restaurants.\n");
        return;
    }

    fclose(file);

    for (i = 0; i < restaurantCount; i++)
    {
        if (restaurants[i].restaurantId <= 0 ||
            !isfinite(restaurants[i].rating) ||
            restaurants[i].rating < 0 || restaurants[i].rating > 5 ||
            memchr(restaurants[i].name, '\0', sizeof(restaurants[i].name)) == NULL ||
            memchr(restaurants[i].cuisine, '\0', sizeof(restaurants[i].cuisine)) == NULL ||
            memchr(restaurants[i].location, '\0', sizeof(restaurants[i].location)) == NULL ||
            memchr(restaurants[i].phone, '\0', sizeof(restaurants[i].phone)) == NULL)
        {
            restaurantCount = 0;
            printf("\nRestaurant data contains an invalid record.\n");
            return;
        }
    }
}


/* ============================================
   ADD RESTAURANT
   ============================================ */

void addRestaurant(void)
{
    Restaurant newRestaurant;

    if (restaurantCount >= MAX_RESTAURANTS)
    {
        printf("\nRestaurant limit reached!\n");
        return;
    }

    newRestaurant.restaurantId = restaurantCount + 1;

    uiHeader("ADD RESTAURANT");

    if (!readLine("Restaurant name: ", newRestaurant.name,
                  sizeof(newRestaurant.name)) ||
        !readLine("Cuisine: ", newRestaurant.cuisine,
                  sizeof(newRestaurant.cuisine)) ||
        !readLine("Location: ", newRestaurant.location,
                  sizeof(newRestaurant.location)) ||
        !readLine("Phone: ", newRestaurant.phone,
                  sizeof(newRestaurant.phone)) ||
        !readFloat("Rating (0 - 5): ", &newRestaurant.rating))
    {
        return;
    }

    if (newRestaurant.rating < 0 ||
        newRestaurant.rating > 5)
    {
        printf("\nInvalid rating. Setting rating to 0.\n");

        newRestaurant.rating = 0;
    }

    restaurants[restaurantCount] = newRestaurant;
    restaurantCount++;

    if (!saveRestaurants())
    {
        restaurantCount--;
        printf("\nRestaurant was not added because it could not be saved.\n");
        return;
    }

    uiHeader("RESTAURANT ADDED");

    printf("Restaurant ID : %d\n", newRestaurant.restaurantId);
    printf("Name          : %s\n", newRestaurant.name);

    printf("\nRestaurant data saved successfully!\n");
}


/* ============================================
   DISPLAY ALL RESTAURANTS
   ============================================ */

void displayRestaurants(void)
{
    int i;

    if (restaurantCount == 0)
    {
        printf("\nNo restaurants available.\n");
        return;
    }

    uiHeader("AVAILABLE RESTAURANTS");
    printf("%-4s %-24s %-18s %6s\n", "ID", "RESTAURANT", "CUISINE", "RATING");
    uiDivider();

    for (i = 0; i < restaurantCount; i++)
    {
        printf("%-4d %-24.24s %-18.18s %5.1f\n",
               restaurants[i].restaurantId,
               restaurants[i].name,
               restaurants[i].cuisine,
               restaurants[i].rating);
        printf("     %s | %s\n", restaurants[i].location, restaurants[i].phone);
    }
    uiDivider();
}


/* ============================================
   FIND RESTAURANT BY ID
   ============================================ */

int findRestaurantById(int restaurantId)
{
    int i;

    for (i = 0; i < restaurantCount; i++)
    {
        if (restaurants[i].restaurantId == restaurantId)
        {
            return i;
        }
    }

    return -1;
}


/* ============================================
   SEARCH RESTAURANT
   ============================================ */

void searchRestaurant(void)
{
    char name[100];

    int found = 0;
    int i;

    uiHeader("SEARCH RESTAURANTS");

    if (!readLine("Restaurant name: ", name, sizeof(name)))
    {
        return;
    }

    for (i = 0; i < restaurantCount; i++)
    {
        if (containsIgnoreCase(restaurants[i].name, name))
        {
            uiHeader("RESTAURANT FOUND");

            printf("Restaurant ID : %d\n",
                   restaurants[i].restaurantId);

            printf("Name          : %s\n",
                   restaurants[i].name);

            printf("Cuisine       : %s\n",
                   restaurants[i].cuisine);

            printf("Location      : %s\n",
                   restaurants[i].location);

            printf("Phone         : %s\n",
                   restaurants[i].phone);

            printf("Rating        : %.1f / 5\n",
                   restaurants[i].rating);

            uiDivider();

            found = 1;
        }
    }

    if (!found)
    {
        printf("\nRestaurant not found.\n");
    }
}