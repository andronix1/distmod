#pragma once

#include "cache.h"
#include "../../prob_eq/prob_eq.h"

bool edsrm_monotonous_create(edsrm_cache_t *cache, prob_eq_t prob_eq, edsrm_cache_pd_info_t *info);
void edsrm_monotonous_free(edsrm_cache_t *cache);