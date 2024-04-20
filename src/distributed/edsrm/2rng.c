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
    if (!edsrm_mnt_create(&cache->lcache, &mnt_cfg)) {
        return false;
    }
    info.a = cfg->extremum;
    info.b = cfg->b;
    info.size = cfg->lsize;
    if (!edsrm_mnt_create(&cache->rcache, &mnt_cfg)) {
        edsrm_mnt_free(&cache->lcache);
        return false;
    }
    edsrm_mnt_cache_segment_t *ls = cache->lcache.segments;
    cache->left_area = ls->v_max * ls->u_width;
    edsrm_mnt_cache_segment_t *rs = cache->rcache.segments;
    cache->right_area = rs->v_max * rs->u_width;
    cache->full_area = cache->left_area + cache->right_area;
    return true;
}

double edsrm_2rng_generate(edsrm_2rng_t *cache, uniform_callable_t *uc) {
    double res;
    while (true) {
        double u_gen = uniform_gen(uc) * cache->full_area;
        edsrm_mnt_t *mnt_cache;
        if (u_gen < cache->left_area) {
            mnt_cache = &cache->lcache;
            u_gen /= cache->left_area;
        } else {
            mnt_cache = &cache->rcache;
            u_gen = (u_gen - cache->left_area) / cache->right_area;
        }
        if (edsrm_mnt_try_generate(&res, u_gen, uc, mnt_cache)) {
            return res;
        }
    }
}

void edsrm_2rng_free(edsrm_2rng_t *cache) {
    edsrm_mnt_free(&cache->lcache);
    edsrm_mnt_free(&cache->rcache);
}