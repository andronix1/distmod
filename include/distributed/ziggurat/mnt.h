#include <stdbool.h>
#include <malloc.h>
#include <math.h>
#include "prob_eq/types.h"
#include "types.h"
#include "dmode.h"

#include "../../uniform/multiplicative.h"
extern multiplicative_rand_gen_t *mul_rand_gen;

#define DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET

#define DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN
#define DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN_CALL multiplicative_rand_gen_generate(mul_rand_gen)

#ifdef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN
    #ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN_CALL
        #error "DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN_CALL undefined"
    #endif
    #define ziggurat_rand_gen(val) DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN_CALL
#else
    #define ziggurat_rand_gen(val) val
#endif

#define DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
#define DISTRAND_ZIGGURAT_USE_IPD true

#ifdef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    #ifndef DISTRAND_ZIGGURAT_USE_IPD
        #error "DISTRAND_ZIGGURAT_USE_IPD undefined"
    #endif
    #define ziggurat_use_ipd(val) DISTRAND_ZIGGURAT_USE_IPD
#else
    #define ziggurat_use_ipd(val) val
#endif

#define DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
#define DISTRAND_ZIGGURAT_IS_RIGHT false

#ifdef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    #ifndef DISTRAND_ZIGGURAT_IS_RIGHT
        #error "DISTRAND_ZIGGURAT_IS_RIGHT undefined"
    #endif
    #define ziggurat_is_right(val) DISTRAND_ZIGGURAT_IS_RIGHT
#else
    #define ziggurat_is_right(val) val
#endif

typedef struct {
    double u_min, u_maj, v_start, v_end;
} ziggurat_mnt_cache_segment_t;

typedef struct {
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_DIRECTION
    const bool is_right;
#endif
    const double u_start;
    const double start;
    const double end;
    const int size;
    const ziggurat_mnt_cache_segment_t * const rows;
    const dfunc_t f;
#ifndef DISTRAND_ZIGGURAT_DISABLE_BOTTOM_OFFSET
    const double bottom_prob;
    const double bottom_offset;
    const bool with_bottom_offset;
#endif
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    const bool use_ipd_for_gen;
#endif
} ziggurat_mnt_t;

typedef struct {
    prob_eq_t prob_eq;
    dfunc_t ipd, pd;
    double start, end;
    int size;
#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_USE_IPD
    bool use_ipd_for_gen;
#endif
} ziggurat_mnt_config_t;

ziggurat_mnt_t *ziggurat_mnt_create(ziggurat_mnt_config_t *config) ;

#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN
inline static bool ziggurat_mnt_try_generate(double *result, const double initial_gen, const gen_callable_t *gc, const ziggurat_mnt_t *cache) 
#else
inline static bool ziggurat_mnt_try_generate(double *result, const double initial_gen, const ziggurat_mnt_t *cache) 
#endif
{
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
    const double v = row->v_start + ziggurat_rand_gen(gen_call(gc)) * (row->v_end - row->v_start);
    return ziggurat_use_ipd(cache->use_ipd_for_gen) ? (ziggurat_is_right(cache->is_right) ? cache->f(v) < u : cache->f(v) > u) : v < cache->f(u);
}

#ifndef DISTRAND_ZIGGURAT_DISABLE_DYNAMIC_RAND_GEN
    double ziggurat_mnt_generate(ziggurat_mnt_t *cache, gen_callable_t *gc);
#else
    double ziggurat_mnt_generate(ziggurat_mnt_t *cache);
#endif
void ziggurat_mnt_free(ziggurat_mnt_t *cache);
