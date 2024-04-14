#pragma once

#include "cache.h"
#include "uniform/uniform.h"

bool edsrm_try_generate(double *result, double first_gen, double (*pd)(), double (*uniform_gen)(), void *uniform_arg, edsrm_cache_t *cache);
double edsrm_generate(double (*pd)(), uniform_t uniform_gen, void *uniform_arg, edsrm_cache_t *cache);