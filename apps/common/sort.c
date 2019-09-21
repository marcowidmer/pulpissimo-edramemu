// Source: https://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Quicksort#C
#include <stdlib.h>
#include "sort.h"

static void swap(void *x, void *y, size_t l)
{
    char *a = x, *b = y, c;
    while(l--) {
        c = *a;
        *a++ = *b;
        *b++ = c;
    }
}

static void sort(char *array, size_t size, int (*cmp)(const void*,const void*), int begin, int end)
{
    if (end > begin) {
        void *pivot = array + begin;
        int l = begin + size;
        int r = end;
        while(l < r) {
            if (cmp(array+l,pivot) <= 0) {
                l += size;
            } else if ( cmp(array+r, pivot) > 0 )  {
                r -= size;
            } else if ( l < r ) {
                swap(array+l, array+r, size);
            }
        }
        l -= size;
        swap(array+begin, array+l, size);
        sort(array, size, cmp, begin, l);
        sort(array, size, cmp, r, end);
    }
}

void qsort(void *array, size_t nitems, size_t size, int (*cmp)(const void*,const void*))
{
    if (nitems > 0) {
        sort(array, size, cmp, 0, (nitems-1)*size);
    }
}
