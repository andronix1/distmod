#pragma once

typedef struct {
    int idx;
    double a;
    double b;
    double (*distr)(double u);
    double (*i_distr)(double u);
    const char *distr_str;
    const char *idistr_str;
    const char *name;
} bench_info_t;

#define DEFINE_BENCH_PD(id, from, to, pd, ipd) double pd_##id(double u) { return pd; } \
    double ipd_##id(double u) { return ipd; } \
    const bench_info_t bi_##id = { \
        .idx = id, \
        .a = from, \
        .b = to, \
        .distr = pd_##id, \
        .i_distr = ipd_##id, \
        .idistr_str = #ipd, \
        .distr_str = #pd, \
        .name = #pd \
    };

#define e 2.7182818284590452353602874713526624977572
#define pi 3.1415926535897932384626433832795028841971
