#pragma once

#include "mnt.h"
#include "prob_eq/types.h"
#include "types.h"

typedef struct {
    edsrm_mnt_t rcache, lcache;
    double leftp, rightp;
} edsrm_2rng_t;

typedef struct {
    prob_eq_t prob_eq;
    dfunc_t pd;
    double a, b, extremum;
    uint32_t lsize, rsize;
} edsrm_2rng_cfg_t;

inline static bool edsrm_2rng_try_generate(double *res, double u_gen with_gc(gc), edsrm_2rng_t *cache)  {
    edsrm_mnt_t *mnt_cache;
    if (u_gen < cache->leftp) {
        mnt_cache = &cache->lcache;
        u_gen /= cache->leftp;
    } else {
        mnt_cache = &cache->rcache;
        u_gen = (u_gen - cache->leftp) / cache->rightp;
    }
    return edsrm_mnt_try_generate(res, u_gen pass_gc(gc), mnt_cache);
}
double edsrm_2rng_generate(edsrm_2rng_t *cache, gen_callable_t *uc);
bool edsrm_2rng_init(edsrm_2rng_t *cache, edsrm_2rng_cfg_t *cfg);
void edsrm_2rng_free(edsrm_2rng_t *cache);
