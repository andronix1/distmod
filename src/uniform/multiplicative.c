#include "uniform/multiplicative.h"

multiplicative_rand_gen_t *multiplicative_rand_gen_create() {
    multiplicative_rand_gen_t *result = malloc(sizeof(multiplicative_rand_gen_t));
    result->a1 = 1; 
    result->a2 = 0; 
    result->a3 = 0;
    return result;
}

double multiplicative_rand_gen_generate(multiplicative_rand_gen_t *rand_gen) {
    uint32_t c = 11973 * rand_gen->a1;
    uint32_t d = (2800 * rand_gen->a1) + (11973 * rand_gen->a2) + (c >> 14);
    rand_gen->a1 = c << 18 >> 18;
    rand_gen->a2 = d << 18 >> 18;
    rand_gen->a3 = ((2842 * rand_gen->a1) + (2800 * rand_gen->a2) + (11973 * rand_gen->a3) + (d >> 14)) << 18 >> 18;
    return (rand_gen->a3 + (rand_gen->a2 + rand_gen->a1 / 16384.0) / 16384.0) / 4096.0 / 4.0;
}

void multiplicative_rand_gen_free(multiplicative_rand_gen_t *rand_gen) {
    free(rand_gen);
}