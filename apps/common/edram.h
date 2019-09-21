#pragma once
#include "io.h"
#include "platform.h"

#define REG_BASE (ABP_BASE + 0x00000)
#define PRAM_BASE (ABP_BASE + 0x10000)
#define EVRAM_BASE (ABP_BASE + 0x20000)

#define SET_CTRL(value) write32(REG_BASE, value)

#define CTRL_RST (1 << 0)
#define CTRL_TEN (1 << 1)
#define CTRL_GEN (1 << 3)

/**
 * Read the refresh interval
 */
#define GET_RFINT() read32(REG_BASE + 4)

/**
 * Set the refresh interval for distributed refresh.
 * Every `value` cycles, one word is refreshed.
 * It takes 128 * `value` cycles to refresh the complete array.
 * Similarly, a given word is refreshed every 128 * `value` cycles.
 */
#define SET_RFINT(value) write32(REG_BASE + 4, value)

/**
 * Map the Pointer RAM & Event RAM regions to a given array.
 * `value` = 0: Write to Pointer RAM & Event RAM of all arrays in parallel.
 * `value` > 0: Write to Pointer RAM & Event RAM of array (`value` - 1).
 */
#define SET_MODSEL(value) write32(REG_BASE + 36, value)

/**
 * Read the current emulation timestamp.
 * This should return the same value as a performance counter in the clock gated domain.
 */
#define GET_SIMTS() read32(REG_BASE + 12)

/**
 * Write the event pointers for a single word into the Pointer RAM.
 * @param row The row (word) index in the array
 * @param ptr Index of the first event in the Event RAM associated with this row
 * @param n Number of subsequent events in the Event RAM associated with this row
 */
#define PRAM_SET(row, ptr, n) write32(PRAM_BASE + (row)*4, ((ptr) << XPAR_EDRAM_0_NEVENTS_LOG) | (n))

/**
 * Write the Event data for a single event into the Event RAM.
 * @param row The index of the event
 * @param tdiff The delta t of the event (in cycles)
 * @param act The action
 * @param bit The index of the bit to flip (0: LSB, 32: MSB)
 */
#define EVRAM_SET(row, tdiff, act, bit) write32(EVRAM_BASE + (row)*4, ((((tdiff) << XPAR_EDRAM_0_ACTSIZE) | (act)) << XPAR_EDRAM_0_WORDSIZE_LOG) | (bit))

// Available actions:
#define ACTION_NONE 0 // Do nothing
#define ACTION_DRT  1 // Flip from 0 to 1 and from 1 to 0 (causes "flickering" inside the memory)
#define ACTION_DRT0 2 // Flip from 0 to 1 only
#define ACTION_DRT1 3 // Flip from 1 to 0 only

/**
 * Reset all registers and internal state (eDRAM contents, Event RAM data and Pointer RAM data are preserved)
 */
#define EDRAM_RESET() SET_CTRL(CTRL_RST);

/**
 * Start the emulator clock and enable clock gating
 */
#define EDRAM_START() SET_CTRL(CTRL_TEN | CTRL_GEN)

/**
 * Stop the emulator clock
 */
#define EDRAM_STOP() SET_CTRL(0)
