#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rt/rt_api.h"
#include "convolution.h"
#include "edram.h"
#include "events.h"
#include "malloc.h"
#include "types.h"

#include "drt.h"

#define RFINT 20 // distributed refresh: refresh one word every 20 cycles ("real" refresh interval: 128*20 cycles)
#define MU 13.0
#define SIGMA 1.7

#define MAX_RFINT 64 // must be >= RFINT
#define ACTION ACTION_DRT1 // flip from 1 to 0

static void print_result(const benchmark_result *result, const void *golden)
{
    convolution_print_result(result->result, golden);
    printf("Execution time:\n");
    printf("%d cycles\n", GET_SIMTS());
    printf("\n");
}

static benchmark_result run(unsigned int rfint)
{
    // Reset all registers
    EDRAM_RESET();

    // Free all previously allocated blocks
    malloc_reset();

    // Set refresh interval (distributed refresh)
    SET_RFINT(rfint);

    // Run the benchmark
    EDRAM_START();
    benchmark_result result = convolution_run();
    EDRAM_STOP();

    return result;
}

/**
 * A demo application running an algorithm on differently generated DRT maps.
 */
int main()
{
    void *golden;
    benchmark_result result;

    // SRAM Reference run (no errors, no refresh)
    printf("REFERENCE\n");
    events_reset(); // clear all events
    result = run(0); // rfint = 0: refresh disabled
    print_result(&result, NULL);

    // Save reference result
    golden = malloc(result.size);
    if (!golden) {
        printf("malloc for golden failed\n");
        abort();
    }
    memcpy(golden, result.result, result.size);

    // Static DRT map:
    printf("STATIC DRT MAP\n");
    events_init(&drts, ACTION, 0); // seed = 0: no shuffle
    result = run(RFINT);
    print_result(&result, golden);

    // Shuffled DRT map:
    printf("SHUFFLED DRT MAP\n");
    events_init(&drts, ACTION, 1); // seed != 0: shuffle
    result = run(RFINT);
    print_result(&result, golden);

    // Random DRT map:
    // Random DRT values are generated from a MU and SIGMA of a log-normal distribution (in cycles).
    // Only DRTs <= MAX_RFINT are written to the event list. Make sure that the refresh interval is always <= MAX_RFINT.
    printf("RANDOM DRT MAP\n");
    events_init_rand(MU, SIGMA, MAX_RFINT, ACTION, 1); // seed = 1
    result = run(RFINT);
    print_result(&result, golden);

    free(golden);

    printf("Done.\n");
    rt_time_wait_us(100000);
}
