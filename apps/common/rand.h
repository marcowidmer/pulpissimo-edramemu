#pragma once

#define RAND_MAX 0x7fffffff

void srand(unsigned int seed);
int rand();
void randn2(float *a, float *b);
float randn();
