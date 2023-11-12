#include "L2Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
CacheL1 L1Cache; // Use the modified Cache structure for L1 cache
CacheL2 L2Cache; // Use the modified Cache structure for L1 cache

/**************** Time Manipulation ***************/
void resetTime() {
    time = 0;
}

uint32_t getTime() {
    return time;
}

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {
    if (address >= DRAM_SIZE - WORD_SIZE + 1)
        exit(-1);

    if (mode == MODE_READ) {
        memcpy(data, &(DRAM[address]), BLOCK_SIZE);
        time += DRAM_READ_TIME;
    }

    if (mode == MODE_WRITE) {
        memcpy(&(DRAM[address]), data, BLOCK_SIZE);
        time += DRAM_WRITE_TIME;
    }
}

/*********************** L1 cache *************************/
void initCache() {
    for (int i = 0; i < L1_NUM_LINES; i++) {
        L1Cache.lines[i].Valid = 0;
        L1Cache.lines[i].Dirty = 0;
        L1Cache.lines[i].Tag = 0;
    }

    for (int i = 0; i < L2_NUM_LINES; i++) {
        L2Cache.lines[i].Valid = 0;
        L2Cache.lines[i].Dirty = 0;
        L2Cache.lines[i].Tag = 0;
    }
    L1Cache.init = 1;
    L2Cache.init = 1;
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
    unsigned int cacheIndex, tag, offset, OldMemAddress;
    uint8_t TempBlock[BLOCK_SIZE];

    /* init cache */
    if (L1Cache.init == 0) {
        initCache();
    }

    CacheLine *Line;
    offset = address % BLOCK_SIZE;
    cacheIndex = (address / BLOCK_SIZE) &  (256-1);
    tag = address / L1_SIZE;
    Line = &(L1Cache.lines[cacheIndex]);

    /* access Cache */
    if (!Line->Valid || Line->Tag != tag) { //if block not present
        // Cache miss, fetch the block from DRAM
        accessL2(address, TempBlock, MODE_READ);

        if (Line->Valid && Line->Dirty) {
           OldMemAddress = (Line->Tag << 14) + (cacheIndex << 6);
            // Write back the old block to L2 if it's dirty
            accessL2(OldMemAddress, Line->Data, MODE_WRITE);
        }

        // Copy the new block to the cache line
        memcpy(Line->Data, TempBlock, BLOCK_SIZE);
        Line->Valid = 1;
        Line->Tag = tag;
        Line->Dirty = 0;
    }

    // Read or write data from/to the cache line
    if (mode == MODE_READ) {
        // Read data from the cache line
        memcpy(data, &(Line->Data[offset]), WORD_SIZE);
        time += L1_READ_TIME; // Increment time for read
    }

    if (mode == MODE_WRITE) {
        // Write data to the cache line
        memcpy(&(Line->Data[offset]), data, WORD_SIZE);
        time += L1_WRITE_TIME; // Increment time for write
        Line->Dirty = 1;
    }
}

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
    unsigned int cacheIndex, tag, MemAddress;
    uint8_t TempBlock[BLOCK_SIZE];

    if (L2Cache.init == 0) {
        initCache();
    }

    CacheLine *Line;
    cacheIndex = (address / BLOCK_SIZE) &  (512-1);
    tag = address / L2_SIZE;
    Line = &(L2Cache.lines[cacheIndex]);

    MemAddress = address >> 6;
    MemAddress = MemAddress << 6;

    /* access Cache */
    if (!Line->Valid || Line->Tag != tag) { //if block not present
        // Cache miss, fetch the block from DRAM
        accessDRAM(MemAddress, TempBlock, MODE_READ);

        if (Line->Valid && Line->Dirty) {
           MemAddress = (Line->Tag << 15) + (cacheIndex << 6);
            // Write back the old block to DRAM if it's dirty
            accessDRAM(MemAddress, Line->Data, MODE_WRITE);
        }


        // Copy the new block to the cache line
        memcpy(Line->Data, TempBlock, BLOCK_SIZE);
        Line->Valid = 1;
        Line->Tag = tag;
        Line->Dirty = 0;
    }

    // Read or write data from/to the cache line
    if (mode == MODE_READ) {
        // Read data from the cache line
        memcpy(data, Line->Data, BLOCK_SIZE);
        time += L2_READ_TIME; // Increment time for read
    }

    if (mode == MODE_WRITE) {
        // Write data to the cache line
        memcpy(Line->Data, data, BLOCK_SIZE);
        time += L2_WRITE_TIME; // Increment time for write
        Line->Dirty = 1;
    }
}

void read(uint32_t address, uint8_t *data) {
    accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
    accessL1(address, data, MODE_WRITE);
}
