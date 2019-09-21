#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "convolution.h"
#include "malloc.h"
#include "util.h"

#include "img.h"

static const int K[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1, };
static const unsigned char Ks = 1;

benchmark_result convolution_run()
{
    unsigned int ix, iy, kx, ky;
    unsigned char p;
    int x, y, c;
    unsigned char *input, *output;

    // Alloc buffers in edram (don't free!!!)
    input = malloc_edram(img_size * img_size);
    output = malloc_edram(img_size * img_size);
    memcpy(input, img_data, img_size * img_size);

    // Execute convolution
    for (ix = 0; ix < img_size; ix++) {
        for (iy = 0; iy < img_size; iy++) {
            c = 0;
            for (kx = 0; kx <= 2 * Ks; kx++) {
                for (ky = 0; ky <= 2 * Ks; ky++) {
                    x = ix + kx - Ks;
                    y = iy + ky - Ks;
                    if (x < 0 || x >= (int) img_size || y < 0 || y >= (int) img_size)
                        p = 0;
                    else
                        p = input[y * img_size + x];

                    c += K[ky * (2 * Ks + 1) + kx] * p;
                }
            }

            if (c > 255)
                c = 255;
            else if (c < 0)
                c = 0;

            output[iy * img_size + ix] = c;
        }
    }

    // Return result
    return (benchmark_result) {output, img_size * img_size};
}

void convolution_print_result(const void *result, const void *golden)
{
    printf("Output:\n");
    print_image((unsigned char *) result, img_size, img_size);
    if (golden) {
        printf("Mean squared error:\n");
        print_diff((unsigned char *) result, (unsigned char *) golden, img_size, img_size);
    }
}
