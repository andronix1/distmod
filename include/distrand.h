#pragma once

#include "uniform/multiplicative.h"
#include "uniform/mt19937_64.h"
#include "distributed/edsrm/2rng.h"
#include "distributed/edsrm/mnt.h"
#include "distributed/ziggurat/mnt.h"
#include "prob_eq/dbd.h"
#include "prob_eq/types.h"

extern inline double gen_call(gen_callable_t *gc);
extern inline bool edsrm_mnt_try_generate(double *result, double first_gen, gen_callable_t *gc, edsrm_mnt_t *cache);