#include "distributed/ziggurat/mnt.h"

double absl(double v) {
    return v > 0 ? v : -v;
}

double ziggurat_mnt_generate(ziggurat_mnt_t *cache, gen_callable_t *gc) {
    double result;
    if (cache->with_bottom_offset) {
        while (true) {
            double gen = gen_call(gc);
            if (gen < cache->bottom_prob) {
                return gen / cache->bottom_prob;
            }
            if (ziggurat_mnt_try_generate(&result, gen, gc, cache)) {
                return result;
            }
        }
    } else {
        double result;
        while (!ziggurat_mnt_try_generate(&result, gen_call(gc), gc, cache));
        return result;
    }
}

double ziggurat_mnt_majorant_area(ziggurat_mnt_t *cache) {
    ziggurat_mnt_cache_segment_t row = cache->rows[0];
    return row.u_maj * (row.v_end - row.v_start) * cache->size;
}

typedef struct {
    ziggurat_mnt_config_t *config;
    bool is_right;
    double bottom_offset;
} ziggurat_mnt_overflow_config_t;

bool ziggurat_mnt_overflow(double initial_height, ziggurat_mnt_overflow_config_t *config) {
    double area = initial_height * (config->config->end - config->config->start);
    double v = config->bottom_offset;
    double u = config->is_right ? config->config->end : config->config->start;
    LOGT("setup: area = %f, initial_height = %f, dir = %s, start = %f, end = %f, size = %d!", area, initial_height, config->is_right ? "right" : "left", config->config->start, config->config->end, config->config->size);
    for (int i = 0; i < config->config->size + 1; i++) {
        v = area / absl((config->is_right ? (u - config->config->start) : (config->config->end - u)));
        u = config->config->ipd(v);
        LOGT("iteration: coords = (%f, %f)!", u, v);
        if (u < config->config->start || u > config->config->end) {
            return true;
        }
    }
    return false;
}

ziggurat_mnt_cache_segment_t *ziggurat_mnt_create_from_initial_height(ziggurat_mnt_config_t *config, double bottom_offset, bool is_right, double initial_height) {
    double area = initial_height * (config->end - config->start);
    ziggurat_mnt_cache_segment_t *rows = malloc(sizeof(ziggurat_mnt_cache_segment_t) * config->size);
    double v = bottom_offset;
    double u = is_right ? config->end : config->start;
    for (int i = 0; i < config->size; i++) {
        double nv = v + area / absl((is_right ? (u - config->start) : (config->end - u)));
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
    double u_start = is_right ? config->start : config->end;
    double initial_height = absl(sv - ev);

    LOGI("creating ziggurat: sv = %f, ev = %f, bottom_offset = %f, dir = %s, u_start = %f, initial_height = %f!", sv, ev, bottom_offset, is_right ? "right" : "left", u_start, initial_height);

    ziggurat_mnt_overflow_config_t ocfg = {
        .bottom_offset = bottom_offset,
        .config = config,
        .is_right = is_right
    };
    if (!config->prob_eq(&initial_height, (bool(*)(double, void*))ziggurat_mnt_overflow, &ocfg)) {
        LOGE("ziggurat prob eq failed!");
        return NULL;
    }

    ziggurat_mnt_t *result = malloc(sizeof(ziggurat_mnt_t));
    result->rows = ziggurat_mnt_create_from_initial_height(config, bottom_offset, is_right, initial_height);
    result->use_ipd_for_gen = config->use_ipd_for_gen;
    result->f = config->use_ipd_for_gen ? config->ipd : config->pd;
    result->is_right = is_right;
    result->size = config->size;
    result->u_start = u_start;
    result->misses = 0;
    result->with_bottom_offset = bottom_offset == 0.0;
    if (result->with_bottom_offset) {
        result->bottom_offset = bottom_offset;
        double bottom_area = (config->end - config->start) * bottom_offset;
        result->bottom_prob = bottom_area / (bottom_area + ziggurat_mnt_majorant_area(result));
    }
    return result;
}

void ziggurat_mnt_free(ziggurat_mnt_t *cache) {
    free(cache->rows);
    free(cache);
}