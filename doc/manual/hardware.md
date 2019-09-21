# Hardware Overview

Figure from DAC paper:

![Hardware block diagram](doc/manual/img/hardware.png)



# RTL Description

This describes the VHDL blocks found in _ips/edram\_emu_.

![RTL block diagram](doc/manual/img/rtl.png)

Left: Memory related

Middle: Control related

Right: Timing related

## TCDM slave
Translates the PULP TCDM signals to a custom protocol.

## MemSlave (`mem_slave.vhd`)
This block allows the eDRAM to supports byte-wise / word-wise write access from by
translating this into a sequence of a read and a write accesses.

## Clock Gate Crossing (`mem_crossing.vhd`)
This block makes sure that all timings are respected across a clock gate
boundary.

## MemMux (`memmux.vhd`)
Translates a single port memory signal into multiple dual port memory signals
going to different arrays.

## MemCtrl (`memctrl.vhd`)
Refresh Controller. It executes distributed refresh and stalls the memory during
the refresh cycles.

## Synchronizer (`synchronizer.vhd`)
Synchronizes memory arrays in case of different overhead. This block decides
if the simulation clock can be advanced.

## MemArray (`memarray_ts.vhd`)
Stores the data and write timestamps. On read, the data and its delta t is
handed to a modifier block to apply events.

## Mod (`mod_ev.vhd`)
This modifier block contains the pointer and event storage and applies events to
the data on read.



# Generics
The following generics are configured in _edram\_emu.vhd_:


## Global Configuration
| Name | Description | Default | Explanation |
| ---- | ----------- | ------- | ----------- |
| TIMERSIZE | bit width of global timer | 32 | 32-bit global timer |

## Sequential Refresh Configuration
The number of arrays that can be put on an FPGA is limited due to increasing
complexity of combinational logic in the Synchronizer block.
To emulate a greater number of arrays, including parallel refresh, arrays can be 
divided into virtual arrays. Virtual arrays are refreshed virtually in parallel.
This is achieved by reading and writing in series. The overhead cycles that
occur due to this are masked by clock gating.

| Name | Description | Default | Explanation |
| ---- | ----------- | ------- | ----------- |
| RFSKIP_LOG | log2 of the number of virtual arrays per array | 5 | 32 virtual arrays per array |
| SKIPSIZE | bit width of RFSKIP_LOG | 3 | 0 <= RFSKIP_LOG < 8 |

## Behavior Configuration
| Name | Description | Default | Explanation |
| ---- | ----------- | ------- | ----------- |
| MEMARRAY_MODEL | | TS | Use timestamp memarray model |
| MOD_MODEL | | EV | Use event modifier model |

## Memory Size Configuration
| Name | Description | Default | Explanation |
| ---- | ----------- | ------- | ----------- |
| NBANKS | number of banks | 4 | 4 banks (with a TCDM slave each) |
| AWIDTH | width of address per bank (in bytes) | 17 | 131072‬ bytes per bank |
| WORDSIZE_LOG | log2 of word size | 5 | 32-bit words |
| ROWS_LOG | log2 of number of rows per array | 12 | 4096 words per array (RFSKIP_LOG is used to divide them into smaller virtual arrays) |

The number of arrays is calculcated automatically. In this case, there are 8 arrays per bank.

## Event Configuration
| Name | Description | Default | Explanation |
| ---- | ----------- | ------- | ----------- |
| NEVENTS_LOG | log2 of number of events per memory array | 12 | 4096 events per array |
| TDIFFSIZE | bit width of delta t in event list | 15 | 15 bits for every delta t |
| ACTSIZE | bit width of action in event list | 2 | 4 possible actions |



# Memory Map

## eDRAM Data
| Base Address | High Address  | Description | Example |
| ------------ | ------------- | ----------- | ------- |
| 0x1C110000 | 0x1C190000 | Emulated eDRAM | Address 0x24: third word in the second bank (assuming 32-bit words) |

Note: Byte-wise / Word-wise access is possible.

## APB Control Port
| Base Address | High Address  | Description | Example |
| ------------ | ------------- | ----------- | ------- |
| 0x1A180000 | 0x1A18FFFF | Registers | Address 0x4: second register |
| 0x1A190000 | 0x1A19FFFF | Pointer RAM (write only) | Address 0x4: second pointer |
| 0x1A1A0000 | 0x1A1AFFFF | Event RAM (write only) | Address 0x4: second event |

### Registers
| Base Address | Name | Description |
| ------------ | ---- | ----------- |
| 0x00 | CTRL | Control register |
| 0x04 | RFINT | Refresh interval in cycles |
| 0x08 | - | Reserved |
| 0x0c | SIMTS | Global timer value |
| 0x10 | - | Reserved |
| 0x14 | - | Reserved |
| 0x18 | - | Reserved |
| 0x1c | - | Reserved |
| 0x20 | - | Reserved |
| 0x24 | MOD_SEL | |

#### CTRL: Control Register
| Bit | Name | Description | Note |
| --- | ---- | ----------- | ---- |
| 0 | RST | 1: Reset eDRAM emulator | All registers are reset, but the integrated RAMs retain their data. |
| 1 | TEN | 1: Enable global timer | Used to start the simulation. |
| 2 | - | Reserved. Set to 0. |
| 3 | GEN | 1: Enable clock gating during overhead cycles | Always enable this together with TEN. |

#### RFINT: Refresh Interval Register
Refresh one word every RFINT cycles (distributed refresh).

##### Example
ROWS_LOG = 2, RFSKIP_LOG = 1, RFINT = 4

Outside view (clock gated domain):

| Cycle | Action |
| ----- | ------ |
| 0 | Refresh read (rows 0, 2). Memory Unavailable. |
| 1 | Refresh write (rows 0, 2). Memory Unavailable. |
| 2 | Normal operation. |
| 3 | Normal operation. |
| 4 | Refresh read (rows 1, 3). Memory Unavailable. |
| 5 | Refresh write (rows 1, 3). Memory Unavailable. |
| 6 | Normal operation. |
| 7 | Normal operation. |
| 8 | Refresh read (rows 0, 2). Memory Unavailable. |

Inside view (assuming no overhead due to modifier block):

| Outside Cycle | Inside cycle | Action | Clock gated? |
| ------------- | ------------ | ------ | ------------ |
| 0 | 0 | Read row 0 | non-gated |
| 0 | 1 | Write row 0 | gated |
| 0 | 2 | Read row 2 | gated |
| 1 | 3 | Write row 2 | non-gated |

#### SIMTS: Global timer value
In cycles. Global timer is enabled through TEN. The timer is stopped during
overhead cycles. This timer can be used instead of an external clock gated
performance counter.

#### MOD_SEL Register
Controls the mapping of the Pointer RAM and Event RAM memory regions.

MOD_SEL = 0: Write to Pointer RAM & Event RAM of all arrays in parallel.

MOD_SEL > 0: Write to Pointer RAM & Event RAM of array (MOD_SEL - 1).

### Pointer RAM
Write only access to the pointer RAM of the array seclected through MOD_SEL.
Number of rows: 2\*\*ROWS_LOG (one row for every word in the array)

Row layout (assuming NEVENTS_LOG = 12):

| 23 ... 12 | 11 ... 0 |
| --------- | -------- |
| Pointer to first event | Number of events |

### Event RAM
Write only access to the event RAM of the array seclected through MOD_SEL.
Number of rows: 2\*\*NEVENTS_LOG

Row layout (assuming TDIFFSIZE = 15, ACTSIZE = 2, WORDSIZE_LOG = 5):

| 21 ... 7 | 6 ... 5 | 4 ... 0 |
| -------- | ------- | ------- |
| Delta t | Action | Bit index |

#### Event Types
| Type | Value (binary) | Description |
| -------- | ------- | ------- |
| None | 00 | Do nothing |
| DRT | 01 | Flip bit |
| DRT0 | 10 | Flip from 0 to 1 |
| DRT1 | 11 | Flip from 1 to 0 |

To stop the values in memory from "flickering", always choose DRT0 or DRT1.
