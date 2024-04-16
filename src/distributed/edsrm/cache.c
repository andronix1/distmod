#include "distributed/edsrm/cache.h"

bool edsrm_cache_init(edsrm_cache_t *result, double du, edsrm_cache_pd_info_t *info) {
    result->segments = malloc(sizeof(edsrm_cache_segment_t) * info->size);
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
        edsrm_cache_segment_t *segment = &result->segments[i];
        segment->u_min = u_min;
        segment->u_width = du;
        segment->v_min = info->pd(u);
        segment->v_max = v_max;
    }

    return true;
}

bool edsrm_is_cache_overflow(double du, edsrm_cache_pd_info_t *info) {
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

void edsrm_cache_free(edsrm_cache_t *cache) {
    free(cache->segments);
}
