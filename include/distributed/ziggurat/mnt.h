#include <stdbool.h>
#include <malloc.h>
#include <math.h>
#include "prob_eq/types.h"
#include "types.h"
#include "../../log.h"
#include "../../uniform/perfomance.h"

#ifdef DISTMOD_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    #define with_bottom_offset(name)
    #define pass_bottom_offset(name)
#else
    #define with_bottom_offset(name) , bool name
    #define pass_bottom_offset(name) , name
#endif

#ifdef DISTMOD_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    #ifndef DISTMOD_ZIGGURAT_USE_IPD
        #error "DISTMOD_ZIGGURAT_USE_IPD undefined"
    #endif
    #define ziggurat_use_ipd(val) DISTMOD_ZIGGURAT_USE_IPD
#else
    #define ziggurat_use_ipd(val) val
#endif

#ifdef DISTMOD_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    #ifndef DISTMOD_ZIGGURAT_IS_RIGHT
        #error "DISTMOD_ZIGGURAT_IS_RIGHT undefined"
    #endif
    #define ziggurat_is_right(val) DISTMOD_ZIGGURAT_IS_RIGHT
    #define with_is_right(name)
    #define pass_is_right(name)
#else
    #define ziggurat_is_right(val) val
    #define with_is_right(name) , bool name
    #define pass_is_right(name) , name
#endif

typedef struct {
    double u_min, u_maj, v_start, v_end;
} ziggurat_mnt_cache_segment_t;

typedef struct {
#ifndef DISTMOD_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    bool is_right;
#endif
    double u_start;
    double start;
    double end;
    int size;
    ziggurat_mnt_cache_segment_t * rows;
    dfunc_t f;
#ifndef DISTMOD_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    double bottom_prob;
    double bottom_offset;
    bool with_bottom_offset;
#endif
#ifndef DISTMOD_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    bool use_ipd_for_gen;
#endif
} ziggurat_mnt_t;

typedef struct {
    prob_eq_t prob_eq;
    dfunc_t ipd, pd;
    double start, end;
    int size;
#ifndef DISTMOD_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    bool use_ipd_for_gen;
#endif
} ziggurat_mnt_config_t;

typedef enum {
	ZIGGURAT_MNT_OK = 0,
	ZIGGURAT_MNT_ERR_PROB_EQ,
	ZIGGURAT_MNT_ERR_CACHE,
#ifdef DISTMOD_ZIGGURAT_DISABLE_BOTTOM_OFFSET
	ZIGGURAT_MNT_ERR_NOT_ADJOINING,
#endif
#ifdef DISTMOD_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
	ZIGGURAT_MNT_ERR_DYN_DIR,
#endif
} ziggurat_mnt_err_t;

ziggurat_mnt_err_t ziggurat_mnt_init(ziggurat_mnt_t *result, ziggurat_mnt_config_t *config);

inline static bool ziggurat_mnt_try_generate(double *result, const double initial_gen with_gc(gc), const ziggurat_mnt_t *cache) {
    const double gen = initial_gen * cache->size;
    const int row_idx = trunc(gen);
    const ziggurat_mnt_cache_segment_t *row = &cache->rows[row_idx];
    double u;
    if (ziggurat_is_right(cache->is_right)) {
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
    const double v = row->v_start + rand_gen(gen_call(gc)) * (row->v_end - row->v_start);
    return ziggurat_use_ipd(cache->use_ipd_for_gen) ? (ziggurat_is_right(cache->is_right) ? cache->f(v) < u : cache->f(v) > u) : v < cache->f(u);
}

double ziggurat_mnt_generate(ziggurat_mnt_t *cache with_gc(gc));
void ziggurat_mnt_free(ziggurat_mnt_t *cache);
