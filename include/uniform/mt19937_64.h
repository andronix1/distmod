#pragma once

#include <stdint.h>
#include <malloc.h>

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL
#define LM 0x7FFFFFFFULL

typedef struct {
    uint64_t *mt;
    int mti;
} mt19937_t;

typedef struct {
    uint64_t seed;
} mt19937_cfg_t;

void mt19937_64_init(mt19937_t* gen, uint64_t seed);
double mt19937_64_generate(mt19937_t* gen);
double mt19937_64_free(mt19937_t* gen);