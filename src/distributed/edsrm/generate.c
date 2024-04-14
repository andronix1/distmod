#include "distributed/edsrm/generate.h"

bool edsrm_try_generate(double *result, double first_gen, double (*pd)(), uniform_t uniform_gen, void *uniform_arg, edsrm_cache_t *cache) {
    int col_idx = first_gen * cache->size;
    edsrm_cache_segment_t *segment = &cache->segments[col_idx];
    double v = uniform_gen(uniform_arg) * segment->v_max;
    double u = segment->u_min + segment->u_width * (cache->size * first_gen - col_idx);
    bool is_under_pd = v <= segment->v_min || v <= pd(u);
    if (is_under_pd) {
        *result = u;
    }
    return is_under_pd;
}

double edsrm_generate(double (*pd)(), uniform_t uniform_gen, void *uniform_arg, edsrm_cache_t *cache) {
    while (true) {
        double u_gen = uniform_gen(uniform_arg);
        int col_idx = u_gen * cache->size;
        edsrm_cache_segment_t *segment = &cache->segments[col_idx];
        double v = uniform_gen(uniform_arg) * segment->v_max;
        double u = segment->u_min + segment->u_width * (cache->size * u_gen - col_idx);
        if (v <= segment->v_min || v <= pd(u)) {
            return u;
        }
    }
}