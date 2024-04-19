#include "distributed/edsrm/2rng.h"

bool edsrm_2rng_create(edsrm_2rng_cache_t *cache, prob_eq_t prob_eq, dfunc_t pd, double rngs[3], uint32_t sizes[2]) {
    edsrm_cache_pd_info_t linfo = {
        .a = rngs[0],
        .b = rngs[1],
        .pd = pd,
        .size = sizes[0]
    };
    edsrm_cache_pd_info_t rinfo = {
        .a = rngs[1],
        .b = rngs[2],
        .pd = pd,
        .size = sizes[1]
    };
    if (!edsrm_mnt_create(&cache->left_cache, prob_eq, &linfo)) {
        return false;
    } 
    if (!edsrm_mnt_create(&cache->right_cache, prob_eq, &rinfo)) {
        edsrm_2rng_free(&cache->left_cache);
        return false;
    }
    edsrm_mnt_cache_segment_t *ls = cache->left_cache.segments;
    cache->left_area = ls->v_max * ls->u_width;
    edsrm_mnt_cache_segment_t *rs = cache->right_cache.segments;
    cache->right_area = rs->v_max * rs->u_width;
    cache->full_area = cache->left_area + cache->right_area;
    return true;
}

double edsrm_2rng_generate(edsrm_2rng_cache_t *cache, dfunc_t pd, uniform_t uniform_gen, void *uniform_arg) {
    double res;
    while (true) {
        double u_gen = uniform_gen(uniform_arg) * cache->full_area;
        edsrm_mnt_cache_t *mnt_cache;
        if (u_gen < cache->left_area) {
            mnt_cache = &cache->left_cache;
            u_gen /= cache->left_area;
        } else {
            mnt_cache = &cache->right_cache;
            u_gen = (u_gen - cache->left_area) / cache->right_area;
        }
        if (edsrm_mnt_try_generate(&res, u_gen, pd, uniform_gen, uniform_arg, mnt_cache)) {
            return res;
        }
    }
}

void edsrm_2rng_free(edsrm_2rng_cache_t *cache) {
    edsrm_mnt_free(&cache->left_cache);
    edsrm_mnt_free(&cache->right_cache);
}