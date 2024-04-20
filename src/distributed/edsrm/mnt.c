#include "distributed/edsrm/mnt.h"

bool edsrm_mnt_from_idu(edsrm_mnt_t *result, double du, edsrm_mnt_pd_info_t *info) {
    result->segments = malloc(sizeof(edsrm_mnt_cache_segment_t) * info->size);
    result->size = info->size;
    result->pd = info->pd;
    
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

bool edsrm_mnt_is_cache_overflow(double du, edsrm_mnt_pd_info_t *info) {
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

bool edsrm_mnt_create(edsrm_mnt_t *cache, edsrm_mnt_cfg_t *cfg) {
    double du = (cfg->pd_info->b - cfg->pd_info->a) / cfg->pd_info->size;
    return cfg->prob_eq(&du, (prob_eq_overflow_t)edsrm_mnt_is_cache_overflow, cfg->pd_info) && edsrm_mnt_from_idu(cache, du, cfg->pd_info);
}

void edsrm_mnt_free(edsrm_mnt_t *cache) {
    free(cache->segments);
}

double edsrm_mnt_generate(edsrm_mnt_t *cache, uniform_callable_t *uc) {
    double result;
    while (!edsrm_mnt_try_generate(&result, uniform_gen(uc), uc, cache));
    return result;
}
