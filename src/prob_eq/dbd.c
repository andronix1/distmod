#include "prob_eq/dbd.h"

bool dbd_prob_eq(double *value, bool (*overflow)(double, void*), void *arg) {
    double result = *value;
    double delta = 1.0;

    while (delta >= result) {
        delta /= 10.0;
    }
    while (overflow(result, arg)) {
        result -= delta;
    }
    int digit_iters = 0;
    while (true) {
        if (overflow(result + delta, arg)) {
            delta /= 10.0;
            if (delta < 10e-18) {
                *value = result;
                return true;
            }
            digit_iters = 0;
        } else {
            digit_iters++;
            if (digit_iters > 10) {
                return false;
            }
            result += delta;
        }
    }
}