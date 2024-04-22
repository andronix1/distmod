#include "benchs.h"

DEFINE_BENCH_PD(1,  0,   1,             2 * u,                                    sqrt(u))
DEFINE_BENCH_PD(2,  0,   0.69314718056, pow(e, u),                                log(1 + u))               //ln2
DEFINE_BENCH_PD(3,  0,   0.8414709848,  1.0 / sqrt(1-u*u),                        sin(u))                   //sin1
DEFINE_BENCH_PD(4,  0,   pi/4,          1 / pow(cos(u), 2),                       atan(u))
DEFINE_BENCH_PD(5,  0,   pi/2,          cos(u),                                   asin(u))
DEFINE_BENCH_PD(6,  0,   1.55740772465, 1.0 / (u * u + 1),                        tan(u))
DEFINE_BENCH_PD(7,  0,   1,             (pi + 1) * pow(u, pi),                    pow(u, 1 / (pi + 1)))
DEFINE_BENCH_PD(8,  0,   1,             3 * pow(u, 2),                            pow(u, 1 / 3.0))
DEFINE_BENCH_PD(9,  0,   1.41421356237, 4 * pow(u, 3) / sqrt(4 * pow(u, 4) + 9),  pow(u * u + 3 * u, 0.25)) //sqrt2
DEFINE_BENCH_PD(10, 0.2, e/5,           5 * pow(log(5 * u), 4) / u,               pow(e, pow(u, 0.2)) / 5)

#define benchs_count 10
const bench_info_t *benchs[benchs_count] = { &bi_1, &bi_2, &bi_3, &bi_4, &bi_5, &bi_6, &bi_7, &bi_8, &bi_9, &bi_10 };

int main() {

}