#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

bool dbd_prob_eq(double *value, bool (*overflow)(double, void*), void *arg);