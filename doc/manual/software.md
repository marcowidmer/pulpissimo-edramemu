# Software Documentation

This describes the software library available in _apps/common_.

## Including the SDK
Add _apps/common_ to your include path, compile all the C files in that
directory and add the following to your own C files:

```c
#include "edram.h"
#include "events.h"
#include "malloc.h"
```

## Controlling the eDRAM Emulation Platform
### Initialize the Event List
#### Remove all Events (Reset)
```c
events_reset();
```

#### Init from DRT Map (Static)
```c
const drt_event_list drts = {
	7, // log2 of rows in DRT map (here: 128)
	126, // number of events
	{ // event list:
		{ // first event: flip bit 1 in row 0 after 10 cycles
		    0, // row
		    1, // bit index
		    10, // delta t (in cycles)
		},
		// ...
	},
};

// 1. Parameter: Pointer to DRT event list
// 2. Parameter: Action type (here: flip-to-0)
// 3. Parameter: Shuffle (here: disabled)
events_init(&drts, ACTION_DRT1, 0);
```

#### Init from DRT Map (Shuffled)
```c
// 3. Parameter: Random seed (vary this parameter to change the DRT map)
events_init(&drts, ACTION_DRT1, 1);
```

#### Init from Random Distribution
```c
// 1. Parameter: Mu for log-normal distribution (unit: cycles)
// 2. Parameter: Sigma for log-normal distribution
// 3. Parameter: Maximal DRT for event list (unit: cycles)
// 4. Parameter: Action type (here: flip-to-0)
// 5. Parameter: Random seed (vary this parameter to change the DRT map)
events_init_rand(MU, SIGMA, MAX_RFINT << (XPAR_EDRAM_0_ROWS_LOG - XPAR_EDRAM_0_RFSKIP_LOG), ACTION_DRT1, 1);
```

### Set Configuration Registers
```c
EDRAM_RESET(); // Reset all registers
SET_RFINT(20); // Interval between rows in cycles. Every word will be refreshed every 128*20 cycles.
```

### Start / Stop emulator
```c
EDRAM_START(); // Start global timer
// Do work
EDRAM_STOP(); // Stop global timer
// Analyze results
```

## Adapting Applications to use the Platform
By default, code & data is stored in the SRAM.

Data can be allocated to the SRAM or eDRAM using our custom allocators,
`malloc_sram()` and `malloc_edram()`. Please use these allocators instead of
the standard `malloc()` function in your code. The only exception is data
which has to be preserved between benchmark runs, use `malloc()` for this.

The custom allocators are very basic and freeing memory is currently not
supported (metadata like the free list would have to be stored seperately in
SRAM to avoid errors, adding a lot of complexity to the implementation). Memory
areas allocated using `malloc_sram()` and `malloc_edram()` MUST NOT BE FREED!
Instead, reset the allocators between benchmark runs using `malloc_reset()`.

Data allocated using the standard `malloc()` function go to a seperate
(standard) heap in SRAM and must be freed normally using `free()`.

To summarize:

| What | Allocate | Free |
| ---- | -------- | ---- |
| Critical data | `malloc_sram()` | No! |
| Error-resilient data | `malloc_edram()` | No! |
| Data to be preserved between runs | `malloc()` | `free()` |
| Reset heaps after run | | `malloc_reset()` |
