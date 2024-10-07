#include <stdbool.h>
#include <malloc.h>
#include <math.h>
#include "prob_eq/types.h"
#include "types.h"
#include "dmode.h"

typedef struct {
    double u_min, u_maj, v_start, v_end;
} ziggurat_mnt_cache_segment_t;

typedef struct {
    const bool is_right;
    const double u_start;
    const int size;
    const ziggurat_mnt_cache_segment_t * const rows;
    const dfunc_t f;
    const double bottom_prob;
    const double bottom_offset;
    const bool with_bottom_offset;
    const bool use_ipd_for_gen;
} ziggurat_mnt_t;

typedef struct {
    prob_eq_t prob_eq;
    dfunc_t ipd, pd;
    double start, end;
    int size;
    bool use_ipd_for_gen;
} ziggurat_mnt_config_t;

ziggurat_mnt_t *ziggurat_mnt_create(ziggurat_mnt_config_t *config) ;

inline static bool ziggurat_mnt_try_generate(double *result, const double initial_gen, const gen_callable_t *gc, const ziggurat_mnt_t *cache) {
    const double gen = initial_gen * cache->size;
    const int row_idx = trunc(gen);
    const ziggurat_mnt_cache_segment_t *row = &cache->rows[row_idx];
    double u;
    if (cache->is_right) {
        u = cache->u_start + (row->u_maj - cache->u_start) * (gen - row_idx);
        *result = u;
        if (u < row->u_min) {
            return true;
        }
    } else {
        u = row->u_maj + (cache->u_start - row->u_maj) * (gen - row_idx);
        *result = u;
        if (u > row->u_min) {
            return true;
        }
    }
    const double v = row->v_start + gen_call(gc) * (row->v_end - row->v_start);
    return cache->use_ipd_for_gen ? (cache->is_right ? cache->f(v) < u : cache->f(v) > u) : v < cache->f(u);
}

double ziggurat_mnt_generate(ziggurat_mnt_t *cache, gen_callable_t *gc);
void ziggurat_mnt_free(ziggurat_mnt_t *cache);