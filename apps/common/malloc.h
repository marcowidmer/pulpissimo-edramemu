#pragma once

void *malloc_sram(unsigned int s);
void *malloc_edram(unsigned int s);
void malloc_reset();
unsigned int malloc_get_sram_size();
unsigned int malloc_get_edram_size();
