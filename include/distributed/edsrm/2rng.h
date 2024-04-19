#pragma once

#include "mnt.h"
#include "prob_eq/types.h"
#include "types.h"

typedef struct {
    edsrm_mnt_cache_t right_cache, left_cache;
    double left_area, right_area, full_area;
} edsrm_2rng_cache_t;

double edsrm_2rng_generate(edsrm_2rng_cache_t *cache, dfunc_t pd, uniform_t uniform_gen, void *uniform_arg);
bool edsrm_2rng_create(edsrm_2rng_cache_t *cache, prob_eq_t prob_eq, dfunc_t pd, double rngs[3], uint32_t sizes[2]);
void edsrm_2rng_free(edsrm_2rng_cache_t *cache);