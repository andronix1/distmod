#pragma once

#include "mnt.h"
#include "prob_eq/types.h"
#include "types.h"

typedef struct {
    edsrm_mnt_t rcache, lcache;
    double left_area, right_area, full_area;
} edsrm_2rng_t;

typedef struct {
    prob_eq_t prob_eq;
    dfunc_t pd;
    double a, b, extremum;
    uint32_t lsize, rsize;
} edsrm_2rng_cfg_t;

double edsrm_2rng_generate(edsrm_2rng_t *cache, uniform_callable_t *uc);
bool edsrm_2rng_create(edsrm_2rng_t *cache, edsrm_2rng_cfg_t *cfg);
void edsrm_2rng_free(edsrm_2rng_t *cache);