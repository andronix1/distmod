#pragma once

#include "multiplicative.h"

#ifdef DISTMOD_DISABLE_DYNAMIC_RAND_GEN
    #ifdef DISTMOD_RAND_GEN_EXTERN
        extern DISTMOD_RAND_GEN_EXTERN;
    #endif
    #ifndef DISTMOD_RAND_GEN_CALL
        #error "DISTMOD_RAND_GEN_CALL undefined"
    #endif

    #define rand_gen(val) DISTMOD_RAND_GEN_CALL
    #define with_gc(name) 
    #define pass_gc(name)
#else
    #define rand_gen(val) val
    #define with_gc(name) , const gen_callable_t *name
    #define pass_gc(name) , name
#endif
