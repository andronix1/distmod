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
    bool rising;
} edsrm_mnt_cache_t;

typedef struct {
    uint32_t size;
    double a;
    double b;
    double(*pd)(double);
} edsrm_cache_pd_info_t;

bool edsrm_mnt_cache_init(edsrm_mnt_cache_t *result, double du, edsrm_cache_pd_info_t *info);
void edsrm_mnt_cache_free(edsrm_mnt_cache_t *cache);
bool edsrm_mnt_is_cache_overflow(double du, edsrm_cache_pd_info_t *info);
bool edsrm_mnt_create(edsrm_mnt_cache_t *cache, prob_eq_t prob_eq, edsrm_cache_pd_info_t *info);
void edsrm_mnt_free(edsrm_mnt_cache_t *cache);
bool edsrm_mnt_try_generate(double *result, double first_gen, dfunc_t pd, double (*uniform_gen)(), void *uniform_arg, edsrm_mnt_cache_t *cache);
double edsrm_mnt_generate(dfunc_t pd, uniform_t uniform_gen, void *uniform_arg, edsrm_mnt_cache_t *cache);