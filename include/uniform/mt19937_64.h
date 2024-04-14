#pragma once

#include <stdint.h>

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL
#define LM 0x7FFFFFFFULL

typedef struct {
    uint64_t mt[NN];
    int mti;
} mt19937_64_t;

void mt19937_64_init(mt19937_64_t* gen, uint64_t seed);
double mt19937_64_generate(mt19937_64_t* gen);