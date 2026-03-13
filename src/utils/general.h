#ifndef BRANZINO_GENERAL_H
#define BRANZINO_GENERAL_H

#include <stddef.h>

typedef void (*PrintFn)(const void *data);

void read_string(const char *prompt, char *buf, size_t size);
void read_password(const char *prompt, char *password, size_t size); // Funzione per leggere password senza echo
int read_int(const char *prompt, int *out);
int read_float(const char *prompt, float *out);

void print_generic(const void *data, PrintFn printer);
void print_int(const void *data);
void print_float(const void *data);
void print_cstring(const void *data);

#endif
