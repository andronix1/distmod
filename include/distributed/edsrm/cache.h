#pragma once

#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>

typedef struct {
    double u_min, v_max, v_min, u_width;
} edsrm_cache_segment_t;

typedef struct {
    uint32_t size;
    edsrm_cache_segment_t *segments;
    bool rising;
} edsrm_cache_t;

typedef struct {
    uint32_t size;
    double a;
    double b;
    double(*pd)(double);
} edsrm_cache_pd_info_t;

bool edsrm_cache_init(edsrm_cache_t *result, double du, edsrm_cache_pd_info_t *info);
void edsrm_cache_free(edsrm_cache_t *cache);

bool edsrm_is_cache_overflow(double du, edsrm_cache_pd_info_t *info);