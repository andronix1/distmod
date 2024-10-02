#pragma once

#include <stdint.h>
#include <malloc.h>

typedef struct {
    uint32_t a1, a2, a3;
} multiplicative_rand_gen_t;

multiplicative_rand_gen_t *multiplicative_rand_gen_create();
double multiplicative_rand_gen_generate(multiplicative_rand_gen_t *rand_gen);
void multiplicative_rand_gen_free(multiplicative_rand_gen_t *rand_gen);