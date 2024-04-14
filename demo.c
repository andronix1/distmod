#include <distrand.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define TIME_BENCHMARK(name, body, count) { \
    clock_t start = clock(); \
    for (long long i = 0; i < count; i++) body \
    clock_t end = clock(); \
    printf(name " time: %f sec for %d gens\n", (double)(end - start) / CLOCKS_PER_SEC, count); \
}

typedef struct {
    int idx;
    double a;
    double b;
    double (*distr)(double u);
    double (*i_distr)(double u);
    const char *name;
} bench_info_t;

#define DEFINE_BENCH_PD(id, from, to, pd, ipd) double pd_##id(double u) { return pd; } \
    double ipd_##id(double u) { return ipd; } \
    const bench_info_t bi_##id = { \
        .idx = id, \
        .a = from, \
        .b = to, \
        .distr = pd_##id, \
        .i_distr = ipd_##id, \
        .name = #pd \
    };

#define e 2.7182818284590452353602874713526624977572
#define pi 3.1415926535897932384626433832795028841971

DEFINE_BENCH_PD(1, 0,   1,             2 * u,                                    sqrt(u))
DEFINE_BENCH_PD(2, 0,   0.69314718056, pow(e, u),                                log(1 + u))               //ln2
DEFINE_BENCH_PD(3, 0,   0.8414709848,  1.0 / sqrt(1-u*u),                        sin(u))                   //sin1
DEFINE_BENCH_PD(4, 0,   1.55740772465, 1.0 / (u * u + 1),                        tan(u))
DEFINE_BENCH_PD(5, 0,   1,             (pi + 1) * pow(u, pi),                    pow(u, 1 / pi))
DEFINE_BENCH_PD(6, 0,   1.41421356237, 4 * pow(u, 3) / sqrt(4 * pow(u, 4) + 9),  pow(u * u + 3 * u, 0.25)) //sqrt2
DEFINE_BENCH_PD(7, 0.2, e/5,           5 * pow(log(5 * u), 4) / u,               pow(e, pow(u, 0.2)) / 5)

#define benchs_count 7
const bench_info_t *benchs[benchs_count] = { &bi_1, &bi_2, &bi_3, &bi_4, &bi_5, &bi_6, &bi_7 };

void run_bi_test(bench_info_t *bi, uniform_t uni_gen, void *uni_arg, long long count) {
    printf("%d). testing %s...\n", bi->idx, bi->name);
    double (*pd)(double u) = bi->distr;
    edsrm_cache_pd_info_t info = {
        .a = bi->a,
        .b = bi->b,
        .pd = pd,
        .size = 330
    };

    edsrm_cache_t cache;
    if (!edsrm_monotonous_create(&cache, dbd_prob_eq, &info)) {
        printf("edsrm_monotonous_create error!\n");
        exit(1);
    }
    TIME_BENCHMARK("edsrm", {
        edsrm_generate(pd, uni_gen, uni_arg, &cache);
    }, count);
    double (*ipd)(double u) = bi->distr;
    TIME_BENCHMARK("idm", {
        ipd(uni_gen(uni_arg));
    }, count);
    edsrm_cache_free(&cache);
}

void run_benchmark(long long count) {
    multiplicative_rand_gen_t multiplicative = multiplicative_rand_gen_create();
    mt19937_64_t mt19937_64;
    mt19937_64_init(&mt19937_64, clock());

    for (int i = 0; i < benchs_count; i++) {
        run_bi_test(benchs[i], (uniform_t)multiplicative_rand_gen_generate, &multiplicative, count);
    }
    for (int i = 0; i < benchs_count; i++) {
        run_bi_test(benchs[i], (uniform_t)mt19937_64_generate, &mt19937_64, count);
    }

    TIME_BENCHMARK("multiplicative", {
        multiplicative_rand_gen_generate(&multiplicative);
    }, count)

    TIME_BENCHMARK("mt19937_64_t", {
        mt19937_64_generate(&mt19937_64);
    }, count)
}

void print_hist(int gens_count, int hist_cols, int hist_len, double (*gen)(double (*)(), uniform_t, void*, void*), double (*pd)(), void *gen_arg) {
    multiplicative_rand_gen_t multiplicative = multiplicative_rand_gen_create();
    double *gens = malloc(sizeof(double) * gens_count);
    double min_val = 10000;
    double max_val = -10000;
    printf("%f\n", max_val);
    printf("%f\n", min_val);
    for (int i = 0; i < gens_count; i++) {
        double val = gen(pd, multiplicative_rand_gen_generate, &multiplicative, gen_arg);
        gens[i] = val;
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    printf("%f\n", max_val);
    printf("%f\n", min_val);
    int *hist = malloc(sizeof(int) * hist_cols);
    memset(hist, 0, sizeof(int) * hist_cols);

    for (int i = 0; i < gens_count; i++) {
        hist[(int)((gens[i] - min_val) / (max_val - min_val) * (hist_cols - 1))]++;
    }

    for (int i = 0; i < hist_cols; i++) {
        for (int j = 0; j < hist[i] / hist_len; j++) {
            putc('*', stdout);
        }
        putc('\n', stdout);
    }

    free(hist);
    free(gens);
}

int main() {
    bench_info_t *bi = benchs[0];

    edsrm_cache_pd_info_t info = {
        .a = bi->a,
        .b = bi->b,
        .pd = bi->distr,
        .size = 330
    };

    edsrm_cache_t cache;
    if (!edsrm_monotonous_create(&cache, dbd_prob_eq, &info)) {
        printf("edsrm_monotonous_create error!\n");
        exit(1);
    }
    print_hist(100000, 10, 20, edsrm_generate, bi->distr, &cache);
    edsrm_cache_free(&cache);

    run_benchmark(10000000);
    return 1;
}