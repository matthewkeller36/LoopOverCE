#ifndef KEY_HELPER
#define KEY_HELPER

#include <stdint.h>
#include <stdbool.h>

#include <keypadc.h>

void init_Keys();
void scan_kb();
void end_kb();

#endif