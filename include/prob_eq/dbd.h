#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "./types.h"

bool dbd_prob_eq(double *value, prob_eq_overflow_t overflow, void *arg);