#ifndef CART_H
#define CART_H

#define MAX_CART_ITEMS 20
#define MAX_ITEM_QUANTITY 99

typedef struct
{
    int itemId;
    int restaurantId;
    char itemName[100];
    int quantity;
    float unitPrice;
    float itemTotal;

} CartItem;

extern CartItem cart[MAX_CART_ITEMS];

/* Display current cart */
void displayCart(void);


/* Add item to cart */
int addToCart(int restaurantId,
              int itemId,
              const char *itemName,
              float unitPrice,
              int quantity);


/* Remove all cart items */
void clearCart(void);

int getCartItemCount(void);
float getCartTotal(void);
int isCartEmpty(void);
int checkoutCart(int customerId);
#endif