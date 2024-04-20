#pragma once

typedef double(*uniform_t)(void*);

typedef struct {
    uniform_t gen;
    void *arg;
} uniform_callable_t;

inline double uniform_gen(uniform_callable_t *uc) { return uc->gen(uc->arg); }