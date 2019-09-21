#include <tgmath.h>
#include "rand.h"

static unsigned long long rand_seed = 1;

void srand(unsigned int seed)
{
    rand_seed = seed;
}

int rand()
{
    rand_seed = rand_seed * 6364136223846793005LL + 1;
    return (rand_seed >> 32) & RAND_MAX;
}

void randn2(float *a, float *b)
{
    // Source: https://rosettacode.org/wiki/Statistics/Normal_distribution
    float x, y, rsq, f;
    do {
        x = 2. * rand() / (float) RAND_MAX - 1.;
        y = 2. * rand() / (float) RAND_MAX - 1.;
        rsq = x * x + y * y;
    } while(rsq >= 1. || rsq == 0.);
    f = sqrt(-2. * log(rsq) / rsq);
    if (a) *a = x * f;
    if (b) *b = y * f;
}

float randn()
{
    float r;
    randn2(&r, NULL);
    return r;
}
