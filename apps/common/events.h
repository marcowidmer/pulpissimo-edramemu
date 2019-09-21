#pragma once
#include "types.h"

void events_reset();
void events_init(const drt_event_list *drts, int action, int shuffle);
void events_init_rand(float mu, float sigma, int max_rfint, int action, int seed);
