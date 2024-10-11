#include "distributed/edsrm/2rng.h"

bool edsrm_2rng_create(edsrm_2rng_t *cache, edsrm_2rng_cfg_t *cfg) {
    edsrm_mnt_pd_info_t info = {
        .a = cfg->a,
        .b = cfg->extremum,
        .pd = cfg->pd,
        .size = cfg->lsize
    };
    edsrm_mnt_cfg_t mnt_cfg = {
        .pd_info = &info,
        .prob_eq = cfg->prob_eq
    };
    if (!(cache->lcache = edsrm_mnt_create(&mnt_cfg))) {
        return false;
    }
    info.a = cfg->extremum;
    info.b = cfg->b;
    info.size = cfg->lsize;
    if (!(cache->rcache = edsrm_mnt_create(&mnt_cfg))) {
        edsrm_mnt_free(cache->lcache);
        return false;
    }
    edsrm_mnt_cache_segment_t *ls = cache->lcache->segments;
    double left_area = ls->v_max * ls->u_width;
    edsrm_mnt_cache_segment_t *rs = cache->rcache->segments;
    double right_area = rs->v_max * rs->u_width;
    double full_area = left_area + right_area;
    cache->leftp = left_area / full_area;
    cache->rightp = right_area / full_area;
    return true;
}

double edsrm_2rng_generate(edsrm_2rng_t *cache, gen_callable_t *gc) {
    double res;
#ifndef DISTRAND_DISABLE_DYNAMIC_RAND_GEN
    while (!edsrm_2rng_try_generate(&res, gen_call(gc), gc, cache));
#else
    while (!edsrm_2rng_try_generate(&res, gen_call(gc), cache));
#endif
    return res;
}

void edsrm_2rng_free(edsrm_2rng_t *cache) {
    edsrm_mnt_free(cache->lcache);
    edsrm_mnt_free(cache->rcache);
}