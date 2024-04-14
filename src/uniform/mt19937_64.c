#include "uniform/mt19937_64.h"

void mt19937_64_init(mt19937_64_t* gen, uint64_t seed) {
    gen->mt[0] = seed;
    for (int i = 1; i < NN; i++) {
        gen->mt[i] =  (6364136223846793005ULL * (gen->mt[i - 1] ^ (gen->mt[i-1] >> 62)) + i);
    }
}

double mt19937_64_generate(mt19937_64_t* gen) {
    int i;
    uint64_t x;

    static const uint64_t mag01[2]={0ULL, MATRIX_A};

    if (gen->mti >= NN) {
        for (i = 0; i < NN - MM; i++) {
            x = (gen->mt[i] & UM) | (gen->mt[i + 1] & LM);
            gen->mt[i] = gen->mt[i + MM] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        }
        for (; i < NN - 1; i++) {
            x = (gen->mt[i] & UM) | (gen->mt[i + 1] & LM);
            gen->mt[i] = gen->mt[i + (MM - NN)] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        }
        x = (gen->mt[NN - 1] & UM) | (gen->mt[0] & LM);
        gen->mt[NN - 1] = gen->mt[MM - 1] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];

        gen->mti = 0;
    }

    x = gen->mt[gen->mti++];

    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);

    return (x >> 11) / 9007199254740992.0;
}