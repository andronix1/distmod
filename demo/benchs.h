#pragma once

#include <distrand.h>
#include "hist.h"
#include "measure_time.h"
#include "config.h"

typedef enum {
    BI_TYPE_MNT,
    BI_TYPE_2RNG,
} bi_type_t;

typedef struct {
    int id;
    double a, b, extremum;
    dfunc_t pd, ipd, pdi;
    const char *pd_str, *ipd_str;
    bi_type_t type;
} bench_info_t;

#define PD_DFUNC_NAME(idx) bi_pd_##idx
#define IPD_DFUNC_NAME(idx) bi_ipd_##idx
#define PDI_DFUNC_NAME(idx) bi_pdi_##idx

#define DFUNC(name, expr) double name(double u) { return expr; }

#define DEFINE_BENCH_INFO(idx, from, to, extremumv, pdv, ipdv, pdiv, typev) \
    DFUNC(PD_DFUNC_NAME(idx), pdv) \
    DFUNC(IPD_DFUNC_NAME(idx), ipdv) \
    DFUNC(PDI_DFUNC_NAME(idx), pdiv) \
    const bench_info_t bi_##idx = { \
        .id = idx, \
        .a = from, \
        .b = to, \
        .extremum = extremumv, \
        .pd = PD_DFUNC_NAME(idx), \
        .ipd = IPD_DFUNC_NAME(idx), \
        .pdi = PDI_DFUNC_NAME(idx), \
        .ipd_str = #ipdv, \
        .pd_str = #pdv, \
        .type = typev \
    }

#define DEFINE_BENCH_MNT(idx, a, b, pd, ipd, pdiv) DEFINE_BENCH_INFO(idx, a, b, 0, pd, ipd, pdiv, BI_TYPE_MNT)
#define DEFINE_BENCH_2RNG(idx, a, b, extremum, pd, ipd, pdiv) DEFINE_BENCH_INFO(idx, a, b, extremum, pd, ipd, pdiv, BI_TYPE_2RNG)

typedef struct { 
    double idm, edsrm;
} bench_result_t;

typedef bool (*bi_edsrm_try_gen_t)(double*, double, gen_callable_t*, void*);
typedef double (*bi_edsrm_gen_t)(void*, gen_callable_t*);
typedef void (*bi_edsrm_free_t)(void*);

typedef struct {
    void *cache;
    bi_edsrm_gen_t gen;
    bi_edsrm_try_gen_t try_gen;
    bi_edsrm_free_t free;
} bi_edsrm_info_t;

void bi_edsrm_info_free(bi_edsrm_info_t *info);
bool bi_create_edsrm(const bench_info_t *bi, bi_edsrm_info_t *info, int m);

bool bi_run(const bench_info_t *bi, gen_callable_t *gc, bench_result_t *res, long gens_count);
bool bi_edsrm_hist(const bench_info_t *bi, gen_callable_t *gc, long gens_count);
bool bi_edsrm_misses(const bench_info_t *bi, gen_callable_t *gc, long gens_count, int *result);


bool bi_create_ziggurat_mnt(const bench_info_t *bi, bi_edsrm_info_t *info, int m);
bool bi_create_edsrm_mnt(const bench_info_t *bi, bi_edsrm_info_t *info, int m);