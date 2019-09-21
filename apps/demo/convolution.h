#pragma once
#include "types.h"

benchmark_result convolution_run();
void convolution_print_result(const void *result, const void *golden);
