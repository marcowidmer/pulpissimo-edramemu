#include <stdlib.h>
#include <tgmath.h>
#include <string.h>
#include <stdio.h>
#include "edram.h"
#include "events.h"
#include "rand.h"
#include "sort.h"
#include "platform.h"

/**
 * Compare two events. Used for sorting.
 */
static int drt_event_comp(const void *a, const void *b)
{
    // sort by row, drt
    const drt_event *f = a;
    const drt_event *s = b;
    // if (f->row > s->row)
    //     return 1;
    // else if (f->row < s->row)
    //     return -1;
    // else
    if (f->drt > s->drt)
        return 1;
    else if (f->drt < s->drt)
        return -1;
    else
        return 0;
}

/**
 * Write the events for a single word in an array.
 */
static void events_write_row(unsigned int row, const drt_event *events, unsigned int nevents, int action, int shuffle, unsigned int *evptr)
{
    if (*evptr + nevents > (1 << XPAR_EDRAM_0_NEVENTS_LOG)) {
        printf("Error: Too many events!\n");
        abort();
    }
    if (shuffle)
        row = rand() % (1 << XPAR_EDRAM_0_ROWS_LOG);
    PRAM_SET(row, *evptr, nevents);
    for (unsigned int i = 0; i < nevents; i++) {
        unsigned int col = shuffle ? rand() % (1 << XPAR_EDRAM_0_WORDSIZE_LOG) : events[i].col;
        EVRAM_SET((*evptr)++, events[i].drt, action, col);
    }
}

/**
 * Write the event list of one array.
 * @param drts Pointer to a DRT map. Must be sorted by row, DRT!!!
 * @param action The action to execute after the DRT has expired
 * @param shuffle If 0: Disable shuffling. If > 0: Enable shuffling.
 */
static void events_write(const drt_event_list *drts, int action, int shuffle)
{
    // qsort(drts->events, drts->nevents, sizeof(drt_event), drt_event_comp);
    unsigned int evptr = 0;
    for (unsigned int offset = 0; offset < (1 << XPAR_EDRAM_0_ROWS_LOG); offset += (1 << drts->rows_log)) {
        unsigned int prev = 0;
        for (unsigned int i = 0; i < drts->nevents; i++) {
            if (drts->events[i].row != drts->events[prev].row) {
                events_write_row(offset + drts->events[prev].row, &drts->events[prev], i - prev, action, shuffle, &evptr);
                prev = i;
            }
        }
        events_write_row(offset + drts->events[prev].row, &drts->events[prev], drts->nevents - prev, action, shuffle, &evptr);
    }
}

/**
 * Create a random DRT map according to a log-normal distribution
 */
static void events_create_rand(drt_event_list *drts, unsigned int max_events, float mu, float sigma, int max)
{
    drts->nevents = 0;
    for (int i = 0; i < (1 << drts->rows_log); i++) {
        unsigned int prev = drts->nevents;
        for (unsigned int j = 0; j < (1 << XPAR_EDRAM_0_WORDSIZE_LOG); j += 2) {
            float r[2];
            randn2(&r[0], &r[1]);
            for (int k = 0; k < 2; k++) {
                int drt = exp(mu + r[k] * sigma);
                if (drt <= max) {
                    if (drts->nevents >= max_events) {
                        printf("Error: Too many random events!\n");
                        abort();
                    }
                    drts->events[drts->nevents++] = (drt_event) {i, j + k, drt};
                }
            }
        }
        qsort(&drts->events[prev], drts->nevents - prev, sizeof(drt_event), drt_event_comp);
    }
}

/**
 * Remove all events from the event list. After this call, the memory will be error-free.
 */
void events_reset()
{
    SET_MODSEL(0);
    memset((char *) PRAM_BASE, 0, 4 << XPAR_EDRAM_0_ROWS_LOG);
    // memset((char *) EVRAM_BASE, 0, 4 << XPAR_EDRAM_0_NEVENTS_LOG);
}

/**
 * Initialize all event lists from a given DRT map, optionally shuffled.
 * @param drts Pointer to a DRT map. Must be sorted by row, DRT!!!
 * @param action The action to execute after the DRT has expired
 * @param shuffle If 0: Disable shuffling. If > 0: Seed used for shuffling.
 */
void events_init(const drt_event_list *drts, int action, int shuffle)
{
    events_reset();
    if (shuffle) {
        // Shuffling enabled
        srand(shuffle);
        for (unsigned int i = 0; i < (1 << (XPAR_EDRAM_0_BANKS_LOG + XPAR_EDRAM_0_ARRAYS_LOG)); i++) {
            // Shuffle and write the given DRT map for every array
            SET_MODSEL(i + 1);
            events_write(drts, action, shuffle);
        }
    } else {
        // No shuffling: write the given DRT map for all arrays at the same time
        SET_MODSEL(0);
        events_write(drts, action, shuffle);
    }
}

/**
 * Initialize all event lists randomly according to a randomly generated DRT map.
 * The DRT map follows a log-normal distribution with MU and SIGMA (in cycles)
 * @param mu Mean of the log-normal distribution
 * @param sigma Standard deviation of the log-normal distribution
 * @param max_rfint The max refresh interval that will ever be used. Used to reduce the number of events in the event list
 * @param action The action to execute after the DRT has expired
 * @param seed The seed used for the RNG
 */
void events_init_rand(float mu, float sigma, int max_rfint, int action, int seed)
{
    events_reset();
    srand(seed);

    // Create a random DRT map for one array according to the log-normal distribution
    drt_event_list *drts = malloc(sizeof(drt_event_list) + (1 << XPAR_EDRAM_0_NEVENTS_LOG) * sizeof(drt_event));
    if (!drts) {
        printf("Malloc failed\n");
        abort();
    }
    drts->rows_log = XPAR_EDRAM_0_ROWS_LOG;
    events_create_rand(drts, 1 << XPAR_EDRAM_0_NEVENTS_LOG, mu, sigma, max_rfint << (XPAR_EDRAM_0_ROWS_LOG - XPAR_EDRAM_0_RFSKIP_LOG));

    // Go through all arrays
    for (unsigned int i = 0; i < (1 << (XPAR_EDRAM_0_BANKS_LOG + XPAR_EDRAM_0_ARRAYS_LOG)); i++) {
        // Write a shuffled version of the generated array
        SET_MODSEL(i + 1);
        events_write(drts, action, 1);
    }

    free(drts);
}
