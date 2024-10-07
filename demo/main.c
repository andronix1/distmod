#define _GNU_SOURCE

#include <stdlib.h>
#include <math.h>
#include <sched.h>
#include "config.h"
#include "benchs.h"

#define e 2.7182818284590452353602874713526624977572
#define pi 3.1415926535897932384626433832795028841971

DEFINE_BENCH_MNT (1,  0,   1,             2 * u,                                    sqrt(u),                   u / 2);
DEFINE_BENCH_MNT (2,  0,   0.69314718056, exp(u),                                   log(1 + u),                log(u));               //ln2
DEFINE_BENCH_MNT (3,  0,   0.8414709848,  1.0 / sqrt(1-u*u),                        sin(u),                    sqrt(1-(1 / u / u)));                   //sin1
DEFINE_BENCH_MNT (4,  0,   pi/4,          1 / pow(cos(u), 2),                       atan(u),                   acos(sqrt(1 / u)));
DEFINE_BENCH_MNT (5,  0,   pi/2,          cos(u),                                   asin(u),                   acos(u)); //Z
DEFINE_BENCH_MNT (6,  0,   1.55740772465, 1.0 / (u * u + 1),                        tan(u),                    sqrt(1 / u - 1)); //Z
DEFINE_BENCH_MNT (7,  0,   1,             (pi + 1) * pow(u, pi),                    pow(u, 1 / (pi + 1)),      pow(u / (pi + 1), 1 / pi));
DEFINE_BENCH_MNT (8,  0,   1,             3 * pow(u, 2),                            pow(u, 1 / 3.0),           pow(u / 3, 1 / 2.0));
DEFINE_BENCH_MNT (9,  0,   1.41421356237, 4 * pow(u, 3) / sqrt(4 * pow(u, 4) + 9),  pow(u * u + 3 * u, 0.25),  1); //Z //sqrt2
DEFINE_BENCH_MNT (10, 0.2, e/5,           5 * pow(log(5 * u), 4) / u,               pow(e, pow(u, 0.2)) / 5,   1); //Z

DEFINE_BENCH_2RNG(11, -1, 1, 0, 3.0/8*(1+u*u), (pow(sqrt(16*u*u-16*u+5)+4*u-2, 2.0/3)-1)/(sqrt(sqrt(16*u*u-16*u+5)+4*u-2)), 1); //Z

// 5, 6, 9, 10
#define benchs_count 6
const bench_info_t *benchs[benchs_count] = { &bi_1, &bi_2, &bi_3, &bi_4, &bi_7, &bi_8 };

typedef struct {
    edsrm_2rng_t *cache;
    gen_callable_t *gc;
} edsrm_2rng_full_t;

void build_edsrm_failed() {
    printf("failed to build edsrm!\n");
    exit(1);
}

double edsrm_2rng_full_gen(edsrm_2rng_full_t *info) {
    return edsrm_2rng_generate(info->cache, info->gc);
}

void run_with_gen(gen_callable_t *gc, bench_result_t *results) {
    for (int i = 0; i < benchs_count; i++) {
        printf("running %d/%d test...\r", i + 1, benchs_count);
        fflush(stdout);
        if (!bi_run(benchs[i], gc, &results[i], DEFAULT_MEASURE_TIME_COUNT)) {
            build_edsrm_failed();
        }
    }
}

void get_misses(gen_callable_t *gc, int *misses) {
    for (int i = 0; i < benchs_count; i++) {
        printf("running misses %d/%d test...\r", i + 1, benchs_count);
        fflush(stdout);
        if (!bi_edsrm_misses(benchs[i], gc, DEFAULT_MISSES_GET_COUNT, &misses[i])) {
            build_edsrm_failed();
        }
    }
}

bool t_by_m(bench_info_t *bi, gen_callable_t *gc, int ms, int me, int step, int height) {
    double *times = calloc(me - ms + 2, sizeof(double));
    double maxt = -__DBL_MAX__;
    for (int i = 0; i < me - ms + 1; i += step) {
        printf("m: %d -> %d...\r", i + ms, me);
        fflush(stdout);
        bi_edsrm_info_t edsrm;
        if (!bi_create_edsrm(bi, &edsrm, i + ms)) {
            return false;
        }

        bi_edsrm_gen_t gen = edsrm.gen;
        void *cache = edsrm.cache;
        double time;
        MEASURE_TIME(time, DEFAULT_M_RNG_COUNT, {
            gen(cache, gc);
        });

        if (time > maxt) maxt = time;
        times[i] = time;

        bi_edsrm_info_free(&edsrm);
    }

    printf("time(%d <= m <= %d):\n", ms, me);
    for (int i = 0; i < me - ms + 1; i += step) {
        printf(" %.05f", times[i]);
    }
    putchar('\n');

    free(times);
    return true;
}

int main() {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);

    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask)) {
        printf("failed to set cpu affinity!\n");
        return 1;
    }

    multiplicative_rand_gen_t *mulrg = multiplicative_rand_gen_create();
    gen_callable_t mul_gc = {
        .arg = mulrg,
        .gen = (gen_t)multiplicative_rand_gen_generate
    };

    bi_edsrm_info_t iz;
    bi_create_ziggurat_mnt(&bi_2, &iz, 500);
    bi_edsrm_info_t ie;
    bi_create_edsrm_mnt(&bi_2, &ie, 500);
        
    double atimez, atimee;
    for (int i = 0; i < 10; i++) {
        double timez;
        MEASURE_TIME(timez, 50000000, {
            iz.gen(iz.cache, &mul_gc);
        });
        atimez += timez;

        double timee;        
        MEASURE_TIME(timee, 50000000, {
            ie.gen(ie.cache, &mul_gc);
        });
        atimee += timee;
    }

    ziggurat_mnt_t *z = iz.cache;
    printf("%f vs %f\n", atimez, atimee);

    return 0;

    mt19937_t mtrg;
    mt19937_64_init(&mtrg, 10);
    gen_callable_t mt_gc = {
        .arg = &mtrg,
        .gen = (gen_t)mt19937_64_generate
    };

    // printf("calculating t(M) for %s...\n", bi_7.ipd_str);
    // t_by_m(&bi_7, &mul_gc, 10, 500, 10, 100);

    for (int i = 0; i < benchs_count; i++) {
        const bench_info_t *bi = benchs[i];
        printf("%s\n", bi->pd_str);
        if (!bi_edsrm_hist(bi, &mul_gc, HIST_GENS_COUNT)) {
            build_edsrm_failed();
        }
    }

    bench_result_t mul_res[benchs_count];
    int misses[benchs_count];
    get_misses(&mul_gc, misses);
    run_with_gen(&mul_gc, mul_res);
    bench_result_t mt_res[benchs_count];
    run_with_gen(&mt_gc, mt_res);

    printf("SAVE THIS TO CSV FILE!!!!\n");
    printf("distribution | inverse distribution | edsrm misses | idm_mul | edsrm_mul | diff_mul | idm_mt19937 | edsrm_mt19937 | diff_mt19937\n");
    for (int i = 0; i < benchs_count; i++) {
        const bench_info_t *b = benchs[i];
        bench_result_t *mr = &mul_res[i];
        bench_result_t *mtr = &mt_res[i];
        printf("%s; %.02f < u < %.02f", b->pd_str, b->a, b->b);
        if (b->type == BI_TYPE_2RNG) {
            printf("; extremum = %.02f", b->extremum);
        }
        printf(" | %s | %d | %.2f | %.2f | %.2f | %.2f | %.2f | %.2f\n", 
            b->ipd_str, 
            misses[i],
            mr->idm, mr->edsrm, mr->idm / mr->edsrm,
            mtr->idm, mtr->edsrm, mtr->idm / mtr->edsrm
        );
    }

    mt19937_64_free(&mtrg);
    multiplicative_rand_gen_free(mulrg);
    return 0;
}