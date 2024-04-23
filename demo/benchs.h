#pragma once

#include <distrand.h>
#include "measure_time.h"

typedef enum {
    BI_TYPE_MNT,
    BI_TYPE_2RNG,
} bi_type_t;

typedef struct {
    int id;
    double a, b, extremum;
    dfunc_t pd, ipd;
    const char *pd_str, *ipd_str;
    bi_type_t type;
} bench_info_t;

#define PD_DFUNC_NAME(idx) bi_pd_##idx
#define IPD_DFUNC_NAME(idx) bi_ipd_##idx

#define DFUNC(name, expr) double name(double u) { return expr; }

#define DEFINE_BENCH_INFO(idx, from, to, extremumv, pdv, ipdv, type) \
    DFUNC(PD_DFUNC_NAME(idx), pdv) \
    DFUNC(IPD_DFUNC_NAME(idx), ipdv) \
    const bench_info_t bi_##id = { \
        .id = idx, \
        .a = from, \
        .b = to, \
        .extremum = extremumv, \
        .pd = PD_DFUNC_NAME(idx), \
        .ipd = IPD_DFUNC_NAME(idx), \
        .ipd_str = #ipdv, \
        .pd_str = #pdv \
    }

#define DEFINE_BENCH_MNT(idx, a, b, pd, ipd) DEFINE_BENCH_INFO(idx, a, b, 0, pd, ipd, BI_TYPE_MNT)
#define DEFINE_BENCH_2RNG(idx, a, b, extremum, pd, ipd) DEFINE_BENCH_INFO(idx, a, b, extremum, pd, ipd, BI_TYPE_2RNG)

#define e 2.7182818284590452353602874713526624977572
#define pi 3.1415926535897932384626433832795028841971

typedef struct { 
    double idm, edsrm;
} bench_result_t;

bool bi_run(bench_info_t *bi, gen_callable_t *gc, bench_result_t *res, long gens_count);