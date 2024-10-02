#include "prob_eq/dbd.h"

bool dbd_prob_eq(double *value, prob_eq_overflow_t overflow, void *arg) {
    double result = *value;
    double delta = 1.0;

    LOGI("result(target delta precision): %f", result);
    while (delta >= result) {
        LOGT("current delta: %f", delta);
        delta /= 10.0;
    }
    LOGI("output delta: %f", delta);
    LOGI("fixing result...");
    while (overflow(result, arg)) {
        LOGT("result: %f -> %f", result, result - delta);
        result -= delta;
        if (result <= delta) {
            delta /= 10;
            if (delta < 10e-16) {
                LOGE("too many fix result iterations!");
                return false;
            }
        }
    }
    int digit_iters = 0;
    while (true) {
        LOGT("iteration: result = %.16f -> %.16f", result, result + delta);
        if (overflow(result + delta, arg)) {
            delta /= 10.0;
            if (delta < 10e-16) {
                LOGI("success!");
                *value = result;
                return true;
            }
            digit_iters = 0;
        } else {
            digit_iters++;
            if (digit_iters > 10) {
                LOGE("too many digit iterations!");
                return false;
            }
            result += delta;
        }
    }
}