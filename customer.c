#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "input.h"
#include "console_ui.h"

Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

int registerCustomer(void);

int saveCustomers(void)
{
    FILE *file;
    int success;

    file = fopen("data/customers.dat", "wb");

    if (file == NULL)
    {
        printf("\nError: Could not save customer data!\n");
        return 0;
    }

    success = fwrite(&customerCount, sizeof(int), 1, file) == 1 &&
              fwrite(customers, sizeof(Customer), customerCount, file) ==
                  (size_t)customerCount;

    if (fclose(file) != 0)
    {
        success = 0;
    }

    if (!success)
    {
        printf("\nError: Customer data could not be fully saved.\n");
    }

    return success;
}

static int loadSampleCustomers(void)
{
    FILE *file = fopen("data/sample_customers.csv", "r");
    Customer sampleCustomers[MAX_CUSTOMERS];
    char line[512];
    int lineNumber = 0;
    int loadedCount = 0;
    int valid = 1;

    if (file == NULL)
    {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *fields[6];
        char *cursor;
        char *delimiter;
        char *end;
        long customerId;
        size_t lineLength;
        int fieldCount = 1;
        int i;

        lineLength = strlen(line);
        if (lineLength > 0 && line[lineLength - 1] != '\n' && !feof(file))
        {
            valid = 0;
            break;
        }

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
        {
            continue;
        }

        if (lineNumber++ == 0)
        {
            continue;
        }

        fields[0] = line;
        cursor = line;
        while ((delimiter = strchr(cursor, ',')) != NULL && fieldCount < 6)
        {
            *delimiter = '\0';
            cursor = delimiter + 1;
            fields[fieldCount++] = cursor;
        }

        if (fieldCount != 6 || strchr(fields[5], ',') != NULL ||
            loadedCount >= MAX_CUSTOMERS)
        {
            valid = 0;
            break;
        }

        customerId = strtol(fields[0], &end, 10);
        if (end == fields[0] || *end != '\0' || customerId <= 0 ||
            customerId > MAX_CUSTOMERS ||
            strlen(fields[1]) >= sizeof(sampleCustomers[loadedCount].name) ||
            strlen(fields[2]) >= sizeof(sampleCustomers[loadedCount].phone) ||
            strlen(fields[3]) >= sizeof(sampleCustomers[loadedCount].address) ||
            strlen(fields[4]) >= sizeof(sampleCustomers[loadedCount].username) ||
            strlen(fields[5]) >= sizeof(sampleCustomers[loadedCount].password))
        {
            valid = 0;
            break;
        }

        for (i = 0; i < loadedCount; i++)
        {
            if (sampleCustomers[i].customerId == customerId ||
                strcmp(sampleCustomers[i].username, fields[4]) == 0)
            {
                valid = 0;
                break;
            }
        }
        if (!valid)
        {
            break;
        }

        sampleCustomers[loadedCount].customerId = (int)customerId;
        strcpy(sampleCustomers[loadedCount].name, fields[1]);
        strcpy(sampleCustomers[loadedCount].phone, fields[2]);
        strcpy(sampleCustomers[loadedCount].address, fields[3]);
        strcpy(sampleCustomers[loadedCount].username, fields[4]);
        strcpy(sampleCustomers[loadedCount].password, fields[5]);
        loadedCount++;
    }

    if (ferror(file))
    {
        valid = 0;
    }

    if (fclose(file) != 0)
    {
        valid = 0;
    }

    if (!valid || loadedCount == 0)
    {
        printf("\nSample customer data is invalid; no demo accounts were loaded.\n");
        return 0;
    }

    memcpy(customers, sampleCustomers, (size_t)loadedCount * sizeof(Customer));
    customerCount = loadedCount;
    printf("\nLoaded %d synthetic demo customer accounts.\n", customerCount);
    return 1;
}

void loadCustomers(void)
{
    FILE *file;
    int i;

    file = fopen("data/customers.dat", "rb");

    if (file == NULL)
    {
        customerCount = 0;
        loadSampleCustomers();
        return;
    }

    if (fread(&customerCount, sizeof(int), 1, file) != 1)
    {
        customerCount = 0;
        fclose(file);
        printf("\nCustomer data is unreadable; starting with no customers.\n");
        return;
    }

    if (customerCount < 0 || customerCount > MAX_CUSTOMERS)
    {
        customerCount = 0;
        fclose(file);
        printf("\nCustomer data has an invalid record count.\n");
        return;
    }

    if (fread(customers, sizeof(Customer), customerCount, file) !=
        (size_t)customerCount)
    {
        customerCount = 0;
        fclose(file);
        printf("\nCustomer data is incomplete; starting with no customers.\n");
        return;
    }

    fclose(file);

    for (i = 0; i < customerCount; i++)
    {
        if (customers[i].customerId <= 0 ||
            memchr(customers[i].name, '\0', sizeof(customers[i].name)) == NULL ||
            memchr(customers[i].phone, '\0', sizeof(customers[i].phone)) == NULL ||
            memchr(customers[i].address, '\0', sizeof(customers[i].address)) == NULL ||
            memchr(customers[i].username, '\0', sizeof(customers[i].username)) == NULL ||
            memchr(customers[i].password, '\0', sizeof(customers[i].password)) == NULL)
        {
            customerCount = 0;
            printf("\nCustomer data contains an invalid record.\n");
            return;
        }
    }
}

void addCustomer(void)
{
    (void)registerCustomer();
}

void displayCustomers(void)
{
    int i;

    if (customerCount == 0)
    {
        printf("\nNo customers available.\n");
        return;
    }

    uiHeader("CUSTOMER LIST");
    printf("%-5s %-24s %-16s\n", "ID", "NAME", "PHONE");
    uiDivider();

    for (i = 0; i < customerCount; i++)
    {
        printf("%-5d %-24.24s %-16.16s\n",
               customers[i].customerId,
               customers[i].name,
               customers[i].phone);
        printf("      %s\n", customers[i].address);
    }
}

int findCustomerById(int customerId)
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (customers[i].customerId == customerId)
        {
            return i;
        }
    }

    return -1;
}
/* ============================================
   CUSTOMER REGISTRATION
   ============================================ */

int isUsernameTaken(const char *username)
{
    int i;

    for (i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].username, username) == 0)
        {
            return 1;
        }
    }

    return 0;
}


int registerCustomer(void)
{
    Customer newCustomer;

    if (customerCount >= MAX_CUSTOMERS)
    {
        printf("\nCustomer limit reached!\n");
        return -1;
    }

    uiHeader("CREATE ACCOUNT");

    newCustomer.customerId = customerCount + 1;

    if (!readLine("Name: ", newCustomer.name, sizeof(newCustomer.name)) ||
        !readLine("Phone: ", newCustomer.phone, sizeof(newCustomer.phone)) ||
        !readLine("Address: ", newCustomer.address, sizeof(newCustomer.address)) ||
        !readLine("Username: ", newCustomer.username, sizeof(newCustomer.username)))
    {
        return -1;
    }

    if (isUsernameTaken(newCustomer.username))
    {
        printf("\nUsername already exists!\n");
        printf("Please choose another username.\n");
        return -1;
    }

    if (!readLine("Password: ", newCustomer.password, sizeof(newCustomer.password)))
    {
        return -1;
    }

    customers[customerCount] = newCustomer;
    customerCount++;

    if (!saveCustomers())
    {
        customerCount--;
        printf("\nAccount was not created because it could not be saved.\n");
        return -1;
    }

    uiHeader("ACCOUNT CREATED");

    printf("Customer ID : %d\n", newCustomer.customerId);
    printf("Name        : %s\n", newCustomer.name);
    printf("Username    : %s\n", newCustomer.username);

    printf("\nPlease login using your username and password.\n");

    return newCustomer.customerId;
}


/* ============================================
   CUSTOMER LOGIN
   ============================================ */

int loginCustomer(void)
{
    char username[50];
    char password[50];
    int i;

    uiHeader("CUSTOMER LOGIN");

    if (!readLine("Username: ", username, sizeof(username)) ||
        !readLine("Password: ", password, sizeof(password)))
    {
        return -1;
    }

    for (i = 0; i < customerCount; i++)
    {
        if (strcmp(customers[i].username, username) == 0 &&
            strcmp(customers[i].password, password) == 0)
        {
            uiHeader("LOGIN SUCCESSFUL");

            printf("Welcome, %s!\n", customers[i].name);
            printf("Customer ID: %d\n", customers[i].customerId);

            return customers[i].customerId;
        }
    }

    printf("\nInvalid username or password!\n");

    return -1;
}