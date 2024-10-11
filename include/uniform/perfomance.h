#pragma once

#include "multiplicative.h"

#define DISTRAND_DISABLE_DYNAMIC_RAND_GEN
#define DISTRAND_RAND_GEN_EXTERN multiplicative_rand_gen_t mul_rand_gen
#define DISTRAND_RAND_GEN_CALL multiplicative_rand_gen_generate(&mul_rand_gen)

#ifdef DISTRAND_DISABLE_DYNAMIC_RAND_GEN
    #ifdef DISTRAND_RAND_GEN_EXTERN
        extern DISTRAND_RAND_GEN_EXTERN;
    #endif
    #ifndef DISTRAND_RAND_GEN_CALL
        #error "DISTRAND_RAND_GEN_CALL undefined"
    #endif

    #define rand_gen(val) DISTRAND_RAND_GEN_CALL
    #define with_gc(name) 
    #define pass_gc(name)
#else
    #define rand_gen(val) val
    #define with_gc(name) , const gen_callable_t *name
    #define pass_gc(name) , name
#endif
