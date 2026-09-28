#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

void uiInitialize(void);
void uiWelcome(void);
void uiDashboardHeader(const char *role, const char *summary);
void uiHeader(const char *title);
void uiDivider(void);
void uiGroup(const char *title);
void uiOption(int choice, const char *label);
void uiPrompt(const char *prompt);
void uiProgressStep(int state, const char *label);

#endif