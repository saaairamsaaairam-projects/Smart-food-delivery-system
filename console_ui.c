#include <stdio.h>

#include "console_ui.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

static int useColor = 0;

void uiInitialize(void)
{
#ifdef _WIN32
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;

    if (output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode) &&
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
    {
        useColor = 1;
    }
#endif
}

void uiWelcome(void)
{
    const char *brandColor = useColor ? "\x1b[1;38;5;45m" : "";
    const char *accentColor = useColor ? "\x1b[38;5;214m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("\n%s  [ SFD ]  SMART FOOD DELIVERY%s\n", brandColor, resetColor);
    printf("%s  -----------------------------------------------%s\n",
        accentColor,
        resetColor);
    printf("  RESTAURANTS  /  ORDERS  /  DELIVERY\n");
}

void uiDashboardHeader(const char *role, const char *summary)
{
    const char *roleColor = useColor ? "\x1b[1;38;5;45m" : "";
    const char *summaryColor = useColor ? "\x1b[38;5;245m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("\n%s%s%s  /  %s%s\n",
        roleColor,
        role,
        resetColor,
        summaryColor,
        summary);
    uiDivider();
}

void uiHeader(const char *title)
{
    const char *titleColor = useColor ? "\x1b[1;38;5;45m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("\n%s> %s%s\n", titleColor, title, resetColor);
    uiDivider();
}

void uiDivider(void)
{
    const char *borderColor = useColor ? "\x1b[38;5;39m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("%s  -----------------------------------------------%s\n",
           borderColor,
           resetColor);
}

void uiGroup(const char *title)
{
    const char *groupColor = useColor ? "\x1b[1;38;5;214m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("\n%s  %s%s\n", groupColor, title, resetColor);
}

void uiOption(int choice, const char *label)
{
    const char *optionColor = useColor ? "\x1b[38;5;214m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("%s%2d%s  %s\n", optionColor, choice, resetColor, label);
}

void uiPrompt(const char *prompt)
{
    const char *promptColor = useColor ? "\x1b[1;38;5;214m" : "";
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("%s%s%s", promptColor, prompt, resetColor);
    fflush(stdout);
}

void uiProgressStep(int state, const char *label)
{
    const char *marker = state == 2 ? "[x]" :
                         (state == 1 ? "[>]" : "[ ]");
    const char *color = !useColor ? "" :
                        (state == 2 ? "\x1b[38;5;42m" :
                         (state == 1 ? "\x1b[1;38;5;214m" :
                          "\x1b[38;5;245m"));
    const char *resetColor = useColor ? "\x1b[0m" : "";

    printf("%s%s%s %s\n", color, marker, resetColor, label);
}