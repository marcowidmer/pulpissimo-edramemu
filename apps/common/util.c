#include <math.h>
#include <stdio.h>
#include "util.h"

float diff_image(const unsigned char *image, const unsigned char *golden, unsigned int width, unsigned int height)
{
    unsigned char max = 0;
    for (unsigned int i = 0; i < width * height; i++) {
        if (golden[i] > max)
            max = golden[i];
    }

    float error = 0;
    for (unsigned int i = 0; i < width * height; i++) {
        unsigned char v = image[i] > max ? max : image[i];
        error += pow(((float) v) - ((float) golden[i]), 2);
    }

    return error / pow(max, 2) / (width * height);
}

void print_image(const unsigned char *image, unsigned int width, unsigned int height)
{
    printf("P2 %d %d %d", width, height, 255);
    for (unsigned int i = 0; i < width * height; i++)
        printf(" %d", image[i]);
    printf("\n");
}

void print_diff(const unsigned char *image, const unsigned char *golden, unsigned int width, unsigned int height)
{
    printf("%d ppm\n", (int) (diff_image(image, golden, width, height) * 1e6));
}
