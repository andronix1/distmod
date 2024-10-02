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
    bool is_right;
    double u_start;
    int size;
    ziggurat_mnt_cache_segment_t *rows;
    dfunc_t f;
    double bottom_prob;
    double bottom_offset;
    bool with_bottom_offset;
    bool use_ipd_for_gen;
    int misses;
} ziggurat_mnt_t;

typedef struct {
    prob_eq_t prob_eq;
    dfunc_t ipd, pd;
    double start, end;
    int size;
    bool use_ipd_for_gen;
} ziggurat_mnt_config_t;

ziggurat_mnt_t *ziggurat_mnt_create(ziggurat_mnt_config_t *config) ;

inline static bool ziggurat_mnt_try_generate(double *result, double initial_gen, gen_callable_t *gc, ziggurat_mnt_t *cache) {
    double gen = initial_gen * cache->size;
    int row_idx = trunc(gen);
    ziggurat_mnt_cache_segment_t *row = &cache->rows[row_idx];
    double u;
    bool is_under_minorant;
    if (cache->is_right) {
        u = cache->u_start + (row->u_maj - cache->u_start) * (gen - row_idx);
        is_under_minorant = u < row->u_min;
    } else {
        u = row->u_maj + (cache->u_start - row->u_maj) * (gen - row_idx);
        is_under_minorant = u > row->u_min;
    }
    if (is_under_minorant) {
        *result = u;
        return true;
    }
    cache->misses += 1;
    double v = row->v_start + gen_call(gc) * (row->v_end - row->v_start);
    bool is_under_distribution = cache->use_ipd_for_gen ? (cache->is_right ? cache->f(v) < u : cache->f(v) > u) : v < cache->f(u);
    if (is_under_distribution) {
        *result = u;
    }
    return is_under_distribution;
}

double ziggurat_mnt_generate(ziggurat_mnt_t *cache, gen_callable_t *gc);
void ziggurat_mnt_free(ziggurat_mnt_t *cache);