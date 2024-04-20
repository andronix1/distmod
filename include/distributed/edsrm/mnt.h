#pragma once

#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include "uniform/uniform.h"
#include "prob_eq/types.h"
#include "types.h"

typedef struct {
    double u_min, v_max, v_min, u_width;
} edsrm_mnt_cache_segment_t;

typedef struct {
    uint32_t size;
    edsrm_mnt_cache_segment_t *segments;
    dfunc_t pd;
    bool rising;
} edsrm_mnt_t;

typedef struct {
    uint32_t size;
    double a, b;
    dfunc_t pd;
} edsrm_mnt_pd_info_t;

typedef struct {
    edsrm_mnt_pd_info_t *pd_info;
    prob_eq_t prob_eq;
} edsrm_mnt_cfg_t;

bool edsrm_mnt_is_cache_overflow(double du, edsrm_mnt_pd_info_t *info);

bool edsrm_mnt_from_idu(edsrm_mnt_t *cache, double idu, edsrm_mnt_pd_info_t *info);
bool edsrm_mnt_create(edsrm_mnt_t *cache, edsrm_mnt_cfg_t *cfg);

void edsrm_mnt_free(edsrm_mnt_t *cache);

inline bool edsrm_mnt_try_generate(double *result, double first_gen, uniform_callable_t *uc, edsrm_mnt_t *cache) {
    int col_idx = first_gen * cache->size;
    edsrm_mnt_cache_segment_t *segment = &cache->segments[col_idx];
    double v = uniform_gen(uc) * segment->v_max;
    double u = segment->u_min + segment->u_width * (cache->size * first_gen - col_idx);
    bool is_under_pd = v <= segment->v_min || v <= cache->pd(u);
    if (is_under_pd) {
        *result = u;
    }
    return is_under_pd;
}

double edsrm_mnt_generate(edsrm_mnt_t *cache, uniform_callable_t *uc);