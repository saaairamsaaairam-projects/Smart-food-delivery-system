#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "console_ui.h"

int readLine(const char *prompt, char *buffer, size_t capacity)
{
    if (buffer == NULL || capacity < 2)
    {
        return 0;
    }

    while (1)
    {
        char *newline;
        size_t index;
        int hasContent = 0;
        int character;

        uiPrompt(prompt);
        fflush(stdout);

        if (fgets(buffer, (int)capacity, stdin) == NULL)
        {
            return 0;
        }

        newline = strchr(buffer, '\n');
        if (newline != NULL)
        {
            *newline = '\0';
        }
        else if (!feof(stdin))
        {
            while ((character = getchar()) != '\n' && character != EOF)
            {
            }

            printf("Input is too long. Please try again.\n");
            continue;
        }

        for (index = 0; buffer[index] != '\0'; index++)
        {
            if (!isspace((unsigned char)buffer[index]))
            {
                hasContent = 1;
                break;
            }
        }

        if (!hasContent)
        {
            printf("Input cannot be empty. Please try again.\n");
            continue;
        }

        return 1;
    }
}

int readInt(const char *prompt, int *value)
{
    char buffer[128];

    while (readLine(prompt, buffer, sizeof(buffer)))
    {
        char *end;
        long parsedValue;

        errno = 0;
        parsedValue = strtol(buffer, &end, 10);

        while (isspace((unsigned char)*end))
        {
            end++;
        }

        if (errno != ERANGE && end != buffer && *end == '\0' &&
            parsedValue >= INT_MIN && parsedValue <= INT_MAX)
        {
            *value = (int)parsedValue;
            return 1;
        }

        printf("Please enter a whole number.\n");
    }

    return 0;
}

int readFloat(const char *prompt, float *value)
{
    char buffer[128];

    while (readLine(prompt, buffer, sizeof(buffer)))
    {
        char *end;
        float parsedValue;

        errno = 0;
        parsedValue = strtof(buffer, &end);

        while (isspace((unsigned char)*end))
        {
            end++;
        }

        if (errno != ERANGE && end != buffer && *end == '\0' &&
            isfinite(parsedValue))
        {
            *value = parsedValue;
            return 1;
        }

        printf("Please enter a valid number.\n");
    }

    return 0;
}