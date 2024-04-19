#pragma once

typedef bool (*prob_eq_overflow_t)(double, void*);
typedef bool(*prob_eq_t)(double *value, prob_eq_overflow_t overflow, void *arg);