#ifndef L1CACHE_H
#define L1CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();
uint32_t getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** L1 cache *************************/

// Constants for L1 cache parameters
#define L1_NUM_LINES L1_SIZE/BLOCK_SIZE    // Number of cache lines (index = 7 bits)

void initCache();
void accessL1(uint32_t, uint8_t *, uint32_t);

typedef struct CacheLine {
    uint8_t Valid;
    uint8_t Dirty;
    uint32_t Tag;
    //uint8_t Data[BLOCK_SIZE];  // Data stored in the cache line
    uint8_t Data[BLOCK_SIZE];   //Data stored in the cache line
} CacheLine;

typedef struct Cache {
    uint32_t init;
    CacheLine lines[L1_NUM_LINES];  // Array of cache lines
} Cache;

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);
void write(uint32_t, uint8_t *);

int binaryToDecimal(uint32_t binary);

#endif
