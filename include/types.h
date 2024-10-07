#pragma once

typedef double (*dfunc_t)(double);

typedef double(*gen_t)(void*);
typedef struct {
    gen_t gen;
    void *arg;
} gen_callable_t;

inline static double gen_call(const gen_callable_t *gc) { return gc->gen(gc->arg); }