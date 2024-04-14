#pragma once

typedef bool(*prob_eq_t)(double *value, bool (*overflow)(double, void*), void *arg);