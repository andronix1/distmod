#include "distributed/edsrm/monotonous.h"

bool edsrm_monotonous_create(edsrm_cache_t *cache, prob_eq_t prob_eq, edsrm_cache_pd_info_t *info) {
    double du = (info->b - info->a) / info->size;
    return prob_eq(&du, edsrm_is_cache_overflow, info) && edsrm_cache_init(cache, du, info);
}

void edsrm_monotonous_free(edsrm_cache_t *cache) {
    free(cache);
}