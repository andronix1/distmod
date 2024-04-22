#include "hist.h"

void print_hist(gen_callable_t *gc, int cols, int height, int gens_count) {
    int *gens = calloc(gens_count, sizeof(double));
    double minv = __DBL_MAX__, maxv = -__DBL_MAX__;
    for (int i = 0; i < gens_count; i++) {
        double gen = gen_call(gc);
        gens[i] = gen;
        if (gen < minv) minv = gen;
        if (gen > maxv) maxv = gen;
    }
    int *hist = calloc(cols, sizeof(int));
    for (int i = 0; i < gens_count; i++) {
        hist[(int)((gens[i] - minv) * cols / (maxv - minv))]++;
    }
    int hist_max = -__INT_MAX__;
    for (int i = 0; i < cols; i++) {
        int v = hist[i];
        if (v > hist_max) hist_max = v;
    }
    free(gens);
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < hist[i] * height / hist_max; i++) {
            putchar('*');
        }
        putchar('\n');
    }
    free(hist);
}