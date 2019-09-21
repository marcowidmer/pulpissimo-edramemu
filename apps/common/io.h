#pragma once

static inline unsigned int read32(unsigned int addr) {
    return *(volatile unsigned int *) addr;
}

static inline void write32(unsigned int addr, unsigned int value) {
    volatile unsigned int *ptr = (volatile unsigned int *) addr;
    *ptr = value;
}
