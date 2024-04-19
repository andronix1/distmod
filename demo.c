#include <distrand.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define hist_gens 10000

#define MEASURE_TIME(body, count, result) { \
    clock_t start = clock(); \
    for (int i = 0; i < count; i++) body \
    clock_t end = clock(); \
    result = (double)(end - start) / CLOCKS_PER_SEC; \
}

#define TIME_BENCHMARK(name, body, count) { \
    double time; \
    MEASURE_TIME(body, count, time) \
    printf(name " time: %f sec for %ld gens\n", time, count); \
}

typedef struct {
    int idx;
    double a;
    double b;
    double (*distr)(double u);
    double (*i_distr)(double u);
    const char *distr_str;
    const char *idistr_str;
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
        .idistr_str = #ipd, \
        .distr_str = #pd, \
        .name = #pd \
    };

#define e 2.7182818284590452353602874713526624977572
#define pi 3.1415926535897932384626433832795028841971

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

typedef struct {
    edsrm_mnt_cache_t cache;
    dfunc_t pd;
    uniform_t uni_gen;
    void *uni_arg;
} full_edsrm_t;

double edsrm_full_gen(full_edsrm_t *edsrm) {
    return edsrm_mnt_generate(edsrm->pd, edsrm->uni_gen, edsrm->uni_arg, &edsrm->cache);
}

typedef struct {
    edsrm_2rng_cache_t cache;
    dfunc_t pd;
    uniform_t uni_gen;
    void *uni_arg;
} full_edsrm_2rng_t;

double edsrm_2rng_full_gen(full_edsrm_2rng_t *edsrm) {
    return edsrm_2rng_generate(&edsrm->cache, edsrm->pd, edsrm->uni_gen, edsrm->uni_arg);
}

typedef struct {
    double idm, edsrm;
} bi_result_t;

void run_bi_test(const bench_info_t *bi, bi_result_t *res, uniform_t uni_gen, void *uni_arg, long long count) {
    printf("%d). measuring %s...\n", bi->idx, bi->name);
    double (*pd)(double u) = bi->distr;
    edsrm_cache_pd_info_t info = {
        .a = bi->a,
        .b = bi->b,
        .pd = pd,
        .size = 330
    };

    full_edsrm_t edsrm = {
        .pd = pd,
        .uni_gen = uni_gen,
        .uni_arg = uni_arg
    };
    if (!edsrm_mnt_create(&edsrm.cache, dbd_prob_eq, &info)) {
        printf("edsrm_mnt_create error!\n");
        exit(1);
    }
    print_hist(hist_gens, 10, 30, edsrm_full_gen, &edsrm);

    MEASURE_TIME({
        edsrm_mnt_generate(pd, uni_gen, uni_arg, &edsrm.cache);
    }, count, res->edsrm);
    double (*ipd)(double u) = bi->distr;
    MEASURE_TIME({
        ipd(uni_gen(uni_arg));
    }, count, res->idm);
    edsrm_mnt_cache_free(&edsrm.cache);
}

void run_benchs_for(bi_result_t *res, uniform_t uni_gen, void* uni_arg, long long count) {
    for (int i = 0; i < benchs_count; i++) {
        run_bi_test(benchs[i], &res[i], uni_gen, uni_arg, count);
    }
}

int ilen(int n) {
    int res = 0;
    do {
        res++;
        n /= 10;
    } while (n > 0);
    return res;
}

int imax(int a, int b) { return a > b ? a : b; }

void run_benchmark(long long count) {
    multiplicative_rand_gen_t multiplicative = multiplicative_rand_gen_create();
    bi_result_t mul_res[benchs_count];
    run_benchs_for(mul_res, (uniform_t)multiplicative_rand_gen_generate, &multiplicative, count);

    mt19937_t mtrd;
    mt19937_64_init(&mtrd, 10);
    bi_result_t mt19937_res[benchs_count];
    run_benchs_for(mt19937_res, (uniform_t)mt19937_64_generate, &mtrd, count);
    mt19937_64_free(&mtrd);

    printf("distribution | inverse distribution | idm_mul | edsrm_mul | diff_mul | idm_mt19937 | edsrm_mt19937 | diff_mt19937\n");
    for (int i = 0; i < benchs_count; i++) {
        const bench_info_t *b = benchs[i];
        bi_result_t *mr = &mul_res[i];
        bi_result_t *mtr = &mt19937_res[i];
        printf("%s | %s | %.2f | %.2f | %.2f | %.2f | %.2f | %.2f\n", 
            b->distr_str, b->idistr_str, 
            mr->idm, mr->edsrm, mr->idm / mr->edsrm,
            mtr->idm, mtr->edsrm, mtr->idm / mtr->edsrm
        );
    }
}

void print_hist(long gens_count, long hist_cols, long hist_len, uniform_t gen, void *gen_arg) {
    double *gens = calloc(gens_count, sizeof(double));
    double min_val = __DBL_MAX__;
    double max_val = -__DBL_MAX__;
    for (int i = 0; i < gens_count; i++) {
        double val = gen(gen_arg);
        gens[i] = val;
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    printf("(%f; %f)\n", min_val, max_val);
    int *hist = calloc(hist_cols + 1, sizeof(int));

    for (int i = 0; i < gens_count; i++) {
        hist[(int)((gens[i] - min_val) * hist_cols / (max_val - min_val))]++;
    }

    int max_hist = 0;
    for (int i = 0; i < hist_cols; i++) {
        if (max_hist < hist[i]) max_hist = hist[i];
    }

    for (int i = 0; i < hist_cols; i++) {
        for (int j = 0; j < hist[i] * hist_len / max_hist; j++) {
            putc('*', stdout);
        }
        putc('\n', stdout);
    }

    free(gens);
    free(hist);
}

double cos2(double u) {
    return cos(u) + 2;
}

void run_uniform_benchmark(long count) {
    {
        printf("multiplicative generator distribution: ");
        multiplicative_rand_gen_t mrd = multiplicative_rand_gen_create();
        print_hist(hist_gens, 10, 30, (uniform_t)multiplicative_rand_gen_generate, &mrd);

        TIME_BENCHMARK("multiplicative", {
            multiplicative_rand_gen_generate(&mrd);
        }, count)
    }
    {
        printf("test distribution: ");
        multiplicative_rand_gen_t mrd = multiplicative_rand_gen_create();
        full_edsrm_2rng_t info = {
            .pd = cos2,
            .uni_gen = multiplicative_rand_gen_generate,
            .uni_arg = &mrd,
        };
        double rng[3] = {0, pi, 2 * pi};
        uint32_t sizes[2] = {330, 330};
        if (!edsrm_2rng_create(&info.cache, dbd_prob_eq, info.pd, rng, sizes)) {
            printf("edsrm_2rng_create error!\n");
            exit(1);
        }

        print_hist(hist_gens, 10, 30, (uniform_t)edsrm_2rng_full_gen, &info);

        edsrm_2rng_free(&info.cache);
    }
    {
        printf("mt19937_t generator distribution: ");
        mt19937_t mtrd;
        mt19937_64_init(&mtrd, 0);
        print_hist(hist_gens, 10, 30, (uniform_t)mt19937_64_generate, &mtrd);

        TIME_BENCHMARK("mt19937_t", {
            mt19937_64_generate(&mtrd);
        }, count)
        mt19937_64_free(&mtrd);
    }
}

int main() {
    long base_count = 1000000;
    run_uniform_benchmark(base_count * 10);
    run_benchmark(base_count);
    return 1;
}
