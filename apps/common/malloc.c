/* A very simple allocator that can only allocate, not free. TODO: keep a free list in SRAM to support freeing. */

#include <stdlib.h>
#include "malloc.h"
#include "platform.h"

typedef struct
{
    char *start;
    char *end;
    char *ptr;
} HEAP;

static HEAP sram_heap = {(char *) SRAM_BASE, (char *) SRAM_END, NULL};
static HEAP edram_heap = {(char *) EDRAM_BASE, (char *) EDRAM_END, NULL};

static void *heap_malloc(HEAP *heap, unsigned int s)
{
    char *base;

    if (s % 4)
        s += 4 - (s % 4);

    if (!heap->ptr)
        heap->ptr = heap->start;

    base = heap->ptr;
    heap->ptr += s;

    if (heap->ptr <= heap->end)
        return base;
    else
        return NULL;
}

static unsigned int heap_get_size(HEAP *heap)
{
    return heap->ptr ? (heap->ptr - heap->start) : 0;
}

static void heap_reset(HEAP *heap)
{
    heap->ptr = NULL;
}

void *malloc_sram(unsigned int s)
{
    return heap_malloc(&sram_heap, s);
}

void *malloc_edram(unsigned int s)
{
    return heap_malloc(&edram_heap, s);
}

void malloc_reset()
{
    heap_reset(&sram_heap);
    heap_reset(&edram_heap);
}

unsigned int malloc_get_sram_size()
{
    return heap_get_size(&sram_heap);
}

unsigned int malloc_get_edram_size()
{
    return heap_get_size(&edram_heap);
}
