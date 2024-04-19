#include "distributed/edsrm/mnt.h"

bool edsrm_mnt_cache_init(edsrm_mnt_cache_t *result, double du, edsrm_cache_pd_info_t *info) {
    result->segments = malloc(sizeof(edsrm_mnt_cache_segment_t) * info->size);
    result->size = info->size;
    
    double pda = info->pd(info->a), pdb = info->pd(info->b);
    bool rising = pda < pdb;

    result->rising = rising;

    double u = rising ? info->b : info->a;
    double segment_area = du * info->pd(u);

    if (segment_area < 0) {
        free(result->segments);
        return false;
    }

    for (int i = 0; i < info->size; i++) {
        double v_max = info->pd(u);
        if (v_max < 0) {
            free(result->segments);
            return false;
        }
        double du = segment_area / v_max;
        double u_min = u;
        if (rising) {
            u_min -= du;
            u -= du;
        } else {
            u += du;
        }
        edsrm_mnt_cache_segment_t *segment = &result->segments[i];
        segment->u_min = u_min;
        segment->u_width = du;
        segment->v_min = info->pd(u);
        segment->v_max = v_max;
    }

    return true;
}

bool edsrm_mnt_is_cache_overflow(double du, edsrm_cache_pd_info_t *info) {
    double pda = info->pd(info->a), pdb = info->pd(info->b);
    bool rising = pda < pdb;

    double s = (rising ? pdb : pda) * du;
    double u = rising ? (info->b - du) : (info->a + du);
    for (int i = 1; i < info->size; i++) {
        double v = info->pd(u);
        if (v < 0) v = -v;
        u += (rising ? -1 : 1) * s / v;
        if (u < info->a || u > info->b) {
            return true;
        }
    }

    return false;
}

void edsrm_mnt_cache_free(edsrm_mnt_cache_t *cache) {
    free(cache->segments);
}

bool edsrm_mnt_create(edsrm_mnt_cache_t *cache, prob_eq_t prob_eq, edsrm_cache_pd_info_t *info) {
    double du = (info->b - info->a) / info->size;
    return prob_eq(&du, edsrm_mnt_is_cache_overflow, info) && edsrm_mnt_cache_init(cache, du, info);
}

void edsrm_mnt_free(edsrm_mnt_cache_t *cache) {
    free(cache->segments);
}

inline bool edsrm_mnt_try_generate(double *result, double first_gen, dfunc_t pd, uniform_t uniform_gen, void *uniform_arg, edsrm_mnt_cache_t *cache) {
    int col_idx = first_gen * cache->size;
    edsrm_mnt_cache_segment_t *segment = &cache->segments[col_idx];
    double v = uniform_gen(uniform_arg) * segment->v_max;
    double u = segment->u_min + segment->u_width * (cache->size * first_gen - col_idx);
    bool is_under_pd = v <= segment->v_min || v <= pd(u);
    if (is_under_pd) {
        *result = u;
    }
    return is_under_pd;
}

double edsrm_mnt_generate(dfunc_t pd, uniform_t uniform_gen, void *uniform_arg, edsrm_mnt_cache_t *cache) {
    while (true) {
        double u_gen = uniform_gen(uniform_arg);
        int col_idx = u_gen * cache->size;
        edsrm_mnt_cache_segment_t *segment = &cache->segments[col_idx];
        double v = uniform_gen(uniform_arg) * segment->v_max;
        double u = segment->u_min + segment->u_width * (cache->size * u_gen - col_idx);
        if (v <= segment->v_min || v <= pd(u)) {
            return u;
        }
    }
}
