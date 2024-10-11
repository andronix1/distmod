#pragma once

#include "uniform/multiplicative.h"
#include "uniform/mt19937_64.h"
#include "distributed/edsrm/2rng.h"
#include "distributed/edsrm/mnt.h"
#include "distributed/ziggurat/mnt.h"
#include "prob_eq/dbd.h"
#include "prob_eq/types.h"

extern inline double gen_call(const gen_callable_t *gc);

#ifndef DISTRAND_DISABLE_DYNAMIC_RAND_GEN
extern inline bool edsrm_mnt_try_generate(double *result, double first_gen, const gen_callable_t *gc, const edsrm_mnt_t *cache);
#else
extern inline bool edsrm_mnt_try_generate(double *result, double first_gen, const edsrm_mnt_t *cache);
#endif