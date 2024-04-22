#pragma once

#include <time.h>

#define bench(output, count, body) { \
        clock_t s = clock(); \
        for (int i = 0; i < count; i++) body \
        clock_t e = clock(); \
        output = (e - s) / (double)CLOCKS_PER_SEC; \
    }