#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

/* Standard headers (recommended) */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keypadc.h>

#include "key_helper.h"

uint8_t* kb_Previous[7];
uint8_t* kb_Falling[7];
uint8_t* kb_Rising[7];

void init_Keys(){
    
    memset_fast(kb_Previous, kb_Data[0], 8);
    memset_fast(kb_Falling, 0, 8);
    memset_fast(kb_Rising, 0, 8);
}

void scan_kb(){
    int i;
    memcpy(kb_Previous, (void*)0xf50010, 8);
    kb_Scan();
    for(i = 0; i < 8; i++){
        *kb_Rising[i] = (kb_Data[i] ^ *kb_Previous[i]) & kb_Data[i];
        *kb_Falling[i] = (kb_Data[i] ^ *kb_Previous[i]) & *kb_Previous[i];
    }
}

void end_kb(){
    free(kb_Previous);
    free(kb_Falling);
    free(kb_Rising);
}