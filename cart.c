#include <stdio.h>
#include <string.h>
#include <math.h>

#include "cart.h"
#include "order.h"
#include "console_ui.h"


CartItem cart[MAX_CART_ITEMS];

int cartItemCount = 0;

int addToCart(int restaurantId,
              int itemId,
              const char *itemName,
              float unitPrice,
              int quantity)
{
    int i;

    if (quantity <= 0 || quantity > MAX_ITEM_QUANTITY)
    {
        printf("\nQuantity must be between 1 and %d.\n", MAX_ITEM_QUANTITY);
        return 0;
    }

    if (!isfinite(unitPrice) || unitPrice <= 0)
    {
        printf("\nInvalid item price.\n");
        return 0;
    }

    if (cartItemCount > 0 && cart[0].restaurantId != restaurantId)
    {
        printf("\nYour cart already contains items from another restaurant.\n");
        printf("Check out or clear your cart before choosing another restaurant.\n");
        return 0;
    }

    /*
       If cart already contains this item,
       increase its quantity instead of creating
       another entry.
    */

    for (i = 0; i < cartItemCount; i++)
    {
        if (cart[i].itemId == itemId &&
            cart[i].restaurantId == restaurantId)
        {
            if (cart[i].quantity > MAX_ITEM_QUANTITY - quantity)
            {
                printf("\nThat quantity exceeds the per-item limit.\n");
                return 0;
            }

            cart[i].quantity += quantity;

            cart[i].itemTotal =
                cart[i].quantity * cart[i].unitPrice;

            printf("\nItem quantity updated in cart!\n");

            return 1;
        }
    }


    /* Check cart capacity */

    if (cartItemCount >= MAX_CART_ITEMS)
    {
        printf("\nCart is full!\n");
        return 0;
    }


    /* Add new item */

    cart[cartItemCount].itemId = itemId;

    cart[cartItemCount].restaurantId =
        restaurantId;

    strcpy(cart[cartItemCount].itemName,
           itemName);

    cart[cartItemCount].quantity =
        quantity;

    cart[cartItemCount].unitPrice =
        unitPrice;

    cart[cartItemCount].itemTotal =
        unitPrice * quantity;

    cartItemCount++;

    uiHeader("ITEM ADDED TO CART");

    printf("Item     : %s\n", itemName);
    printf("Quantity : %d\n", quantity);
    printf("Price    : Rs. %.2f\n", unitPrice);
    printf("Total    : Rs. %.2f\n",
           unitPrice * quantity);

    return 1;
}


/* ============================================
   DISPLAY CART
   ============================================ */

void displayCart(void)
{
    int i;
    float total = 0;

    if (cartItemCount == 0)
    {
        uiHeader("YOUR CART");

        printf("\nYour cart is empty.\n");

        return;
    }


        uiHeader("YOUR CART");
        printf("%-4s %-24s %5s %10s %12s\n",
            "#", "ITEM", "QTY", "EACH", "TOTAL");
        uiDivider();

    for (i = 0; i < cartItemCount; i++)
    {
         printf("%-4d %-24.24s %5d %10.2f %12.2f\n",
             i + 1,
             cart[i].itemName,
             cart[i].quantity,
             cart[i].unitPrice,
             cart[i].itemTotal);

        total += cart[i].itemTotal;
    }

        uiDivider();
        printf("TOTAL: Rs. %.2f\n", total);
}


/* ============================================
   CLEAR CART
   ============================================ */

void clearCart(void)
{
    cartItemCount = 0;

    printf("\nCart cleared successfully!\n");
}


/* ============================================
   GET CART ITEM COUNT
   ============================================ */

int getCartItemCount(void)
{
    return cartItemCount;
}


/* ============================================
   GET CART TOTAL
   ============================================ */

float getCartTotal(void)
{
    int i;

    float total = 0;

    for (i = 0; i < cartItemCount; i++)
    {
        total += cart[i].itemTotal;
    }

    return total;
}


/* ============================================
   CHECK CART EMPTY
   ============================================ */

int isCartEmpty(void)
{
    if (cartItemCount == 0)
    {
        return 1;
    }

    return 0;
}


int checkoutCart(int customerId)
{
    if (isCartEmpty())
    {
        printf("\nYour cart is empty.\n");
        return 0;
    }

    createOrder(customerId);
    return isCartEmpty();
}