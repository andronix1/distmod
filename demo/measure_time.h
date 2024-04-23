#pragma once

#include <time.h>

#define MEASURE_TIME(output, count, body) { \
        clock_t start = clock(); \
        for (int i = 0; i < count; i++) body \
        clock_t end = clock(); \
        output = (end - start) / (double)CLOCKS_PER_SEC; \
    }