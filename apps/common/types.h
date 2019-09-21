#pragma once
#include <stdint.h>

typedef struct {
    uint16_t row;
    uint16_t col;
    uint16_t drt;
} drt_event;

typedef struct {
    unsigned int rows_log;
    unsigned int nevents;
    drt_event events[];
} drt_event_list;

typedef struct {
    void *result;
    unsigned int size;
} benchmark_result;
