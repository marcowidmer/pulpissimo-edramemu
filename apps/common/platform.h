#pragma once

// SRAM region (1024KiB)
#define SRAM_BASE  (0x1C010000 + 0x80000)
#define SRAM_END   0x1C110000

// eDRAM region (512KiB)
#define EDRAM_BASE 0x1C110000
#define EDRAM_END  0x1C190000

// Register base
#define ABP_BASE   0x1A180000

#define XPAR_EDRAM_0_BANKS_LOG    2  // 4 banks
#define XPAR_EDRAM_0_ARRAYS_LOG   3  // 8 arrays per bank
#define XPAR_EDRAM_0_WORDSIZE_LOG 5  // 32 bits per word
#define XPAR_EDRAM_0_ROWS_LOG     12 // 4096 words per bank (split into 32 virtual arrays of 128 words each)
#define XPAR_EDRAM_0_TDIFFSIZE    15
#define XPAR_EDRAM_0_ACTSIZE      2
#define XPAR_EDRAM_0_NEVENTS_LOG  12 // max 4096 events per array
#define XPAR_EDRAM_0_RFSKIP_LOG   5  // 32 virtual arrays per array (128 words each)
