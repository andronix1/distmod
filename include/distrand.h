#pragma once

#include "uniform/uniform.h"
#include "uniform/multiplicative.h"
#include "uniform/mt19937_64.h"
#include "distributed/edsrm/2rng.h"
#include "distributed/edsrm/mnt.h"
#include "prob_eq/dbd.h"
#include "prob_eq/types.h"

extern inline double uniform_gen(uniform_callable_t *uc);
extern inline bool edsrm_mnt_try_generate(double *result, double first_gen, uniform_callable_t *uc, edsrm_mnt_t *cache);