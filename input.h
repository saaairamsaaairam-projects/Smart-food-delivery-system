#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

int readLine(const char *prompt, char *buffer, size_t capacity);
int readInt(const char *prompt, int *value);
int readFloat(const char *prompt, float *value);

#endif