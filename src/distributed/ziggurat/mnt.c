#include "distributed/ziggurat/mnt.h"
#include <string.h>

double absl(double v) {
    return v > 0 ? v : -v;
}

double ziggurat_mnt_generate(ziggurat_mnt_t *cache with_gc(gc)) {
    double result;
    
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    if (cache->with_bottom_offset) {
        while (true) {
            double gen = gen_call(gc);
	        double bp = cache->bottom_prob;
            if (gen < bp) {
                return gen / bp * (cache->end - cache->start) + cache->start;
            }
            gen -= bp;
            gen /= 1 - bp;
            if (ziggurat_mnt_try_generate(&result, gen, gc, cache)) {
                return result;
            }
        }
    }
#endif
    
    while (!ziggurat_mnt_try_generate(&result, rand_gen(gen_call(gc)) pass_gc(gc), cache));
    return result;
}

double ziggurat_mnt_majorant_area(ziggurat_mnt_cache_segment_t *rows, size_t size) {
    ziggurat_mnt_cache_segment_t row = rows[0];
    return row.u_maj * (row.v_end - row.v_start) * size;
}

typedef struct {
    ziggurat_mnt_config_t *config;
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    bool is_right;
#endif
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    double bottom_offset;
#endif
} ziggurat_mnt_overflow_config_t;

bool ziggurat_mnt_overflow(double initial_height, ziggurat_mnt_overflow_config_t *config) {
    double area = initial_height * (config->config->end - config->config->start);
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    double v = config->bottom_offset;
#else
    double v = 0.0;
#endif
    double u = ziggurat_is_right(config->is_right) ? config->config->end : config->config->start;
    for (int i = 0; i < config->config->size; i++) {
        double nv = v + area / absl((ziggurat_is_right(config->is_right) ? (u - config->config->start) : (config->config->end - u)));
        double nu = config->config->ipd(nv);
        if (u < config->config->start || u > config->config->end) {
            return true;
        }
        v = nv;
        u = nu;
    }
    return false;
}

ziggurat_mnt_cache_segment_t *ziggurat_mnt_create_from_initial_height(ziggurat_mnt_config_t *config with_bottom_offset(bottom_offset) with_is_right(is_right), double initial_height) {
    double area = initial_height * (config->end - config->start);
    ziggurat_mnt_cache_segment_t *rows = malloc(sizeof(ziggurat_mnt_cache_segment_t) * config->size);
    #ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
        double v = bottom_offset;
    #else
        double v = 0.0;
    #endif
    double u = ziggurat_is_right(is_right) ? config->end : config->start;
    for (int i = 0; i < config->size; i++) {
        double nv = v + area / absl((ziggurat_is_right(is_right) ? (u - config->start) : (config->end - u)));
        double nu = config->ipd(nv);
        ziggurat_mnt_cache_segment_t *row = &rows[i];
        row->u_maj = u;
        row->u_min = nu;
        row->v_start = v;
        row->v_end = nv;
        v = nv;
        u = nu;
    }
    return rows;
}

ziggurat_mnt_t *ziggurat_mnt_create(ziggurat_mnt_config_t *config) {
    double sv = config->pd(config->start);
    double ev = config->pd(config->end);
    double bottom_offset = sv > ev ? ev : sv;

    bool is_right = sv > ev;
#ifdef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    if (DISTRAND_ZIGGURAT_IS_RIGHT != is_right) {
        // TODO: log invalid direction
        return NULL;
    }
#endif
    double u_start = ziggurat_is_right(is_right) ? config->start : config->end;
    double initial_height = absl(sv - ev);

    bool with_bottom_offset = bottom_offset > __DBL_MIN__;
#ifdef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    if (with_bottom_offset) {
        // TODO: log invalid offset
        return NULL;
    }
#endif

    ziggurat_mnt_overflow_config_t ocfg = {
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
        .bottom_offset = bottom_offset,
#endif
        .config = config,
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
        .is_right = is_right
#endif
    };
    if (!config->prob_eq(&initial_height, (bool(*)(double, void*))ziggurat_mnt_overflow, &ocfg)) {
        return NULL;
    }

    ziggurat_mnt_t *result = malloc(sizeof(ziggurat_mnt_t));
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    double bottom_area = (config->end - config->start) * bottom_offset;
#endif
    ziggurat_mnt_cache_segment_t *rows = ziggurat_mnt_create_from_initial_height(config pass_bottom_offset(bottom_offset) pass_is_right(is_right), initial_height);
    result->rows = rows;
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    result->use_ipd_for_gen = config->use_ipd_for_gen;
#endif
    result->f = ziggurat_use_ipd(config->use_ipd_for_gen) ? config->ipd : config->pd;
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    result->is_right = is_right;
#endif
    result->size = config->size;
    result->u_start = u_start;
    result->start = config->start;
    result->end = config->end;
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    result->with_bottom_offset = with_bottom_offset;
    result->bottom_offset = bottom_offset;
    result->bottom_prob = bottom_area / (bottom_area + ziggurat_mnt_majorant_area(rows, config->size));
#endif
    return result;
}

void ziggurat_mnt_free(ziggurat_mnt_t *cache) {
    free(cache->rows);
    free(cache);
}
