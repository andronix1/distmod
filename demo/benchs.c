#include "benchs.h"

void bi_edsrm_info_free(bi_edsrm_info_t *info) {
    info->free(info->cache);
}

bool bi_create_edsrm_mnt(const bench_info_t *bi, bi_edsrm_info_t *info, int m) {
    edsrm_mnt_pd_info_t pd_info = {
        .a = bi->a,
        .b = bi->b,
        .pd = bi->pd,
        .size = m
    };
    edsrm_mnt_cfg_t cfg = {
        .pd_info = &pd_info,
        .prob_eq = dbd_prob_eq
    };

    if (!(info->cache = edsrm_mnt_create(&cfg))) {
        return false;
    }
    info->gen = (bi_edsrm_gen_t)edsrm_mnt_generate;
    info->free = (bi_edsrm_free_t)edsrm_mnt_free;
    info->try_gen = (bi_edsrm_try_gen_t)edsrm_mnt_try_generate;
    return true;
}

bool bi_create_ziggurat_mnt(const bench_info_t *bi, bi_edsrm_info_t *info, int m) {
    ziggurat_mnt_config_t cfg = {
        .start = bi->a,
        .end = bi->b,
        .ipd = bi->pdi,
        .pd = bi->pd,
        .prob_eq = dbd_prob_eq,
        .size = m,
        .use_ipd_for_gen = true
    };

    if (!(info->cache = ziggurat_mnt_create(&cfg))) {
        return false;
    }
    info->gen = (bi_edsrm_gen_t)ziggurat_mnt_generate;
    info->free = (bi_edsrm_free_t)ziggurat_mnt_free;
    info->try_gen = NULL;
    return true;
}

bool bi_create_edsrm_2rng(const bench_info_t *bi, bi_edsrm_info_t *info, int m) {
    edsrm_2rng_cfg_t cfg = {
        .a = bi->a,
        .b = bi->b,
        .pd = bi->pd,
        .lsize = m,
        .rsize = m,
        .prob_eq = dbd_prob_eq,
        .extremum = bi->extremum,
    };

    info->cache = malloc(sizeof(edsrm_2rng_t));
    if (!edsrm_2rng_create(info->cache, &cfg)) {
        free(info->cache);
        return false;
    }
    info->gen = (bi_edsrm_gen_t)edsrm_2rng_generate;
    info->free = (bi_edsrm_free_t)edsrm_2rng_free;
    info->try_gen = (bi_edsrm_try_gen_t)edsrm_2rng_try_generate;
    return true;
}

bool bi_create_edsrm(const bench_info_t *bi, bi_edsrm_info_t *info, int m) {
    return (bi->type == BI_TYPE_MNT ? bi_create_ziggurat_mnt : bi_create_edsrm_2rng)(bi, info, m);
}

bool bi_idm_run(const bench_info_t *bi, gen_callable_t *gc, double *res, long gens_count) {
    dfunc_t ipd = bi->ipd;
    MEASURE_TIME(*res, gens_count, {
        ipd(gen_call(gc));
    });
    return true;
}

bool bi_edsrm_run(const bench_info_t *bi, gen_callable_t *gc, double *res, long gens_count) {
    bi_edsrm_info_t info;
    if (!bi_create_edsrm(bi, &info, DEFAULT_CACHE_SIZE)) {
        return false;
    }

    void *cache = info.cache;
    bi_edsrm_gen_t gen = info.gen;

    MEASURE_TIME(*res, gens_count, {
        gen(cache, gc);
    });

    bi_edsrm_info_free(&info);

    return true;
}

typedef struct {
    bi_edsrm_info_t *info;
    gen_callable_t *gc;
} full_edsrm_t;

double full_edsrm_gen(full_edsrm_t *edsrm) {
    return (edsrm->info->gen)(edsrm->info->cache, edsrm->gc);
}

bool bi_edsrm_hist(const bench_info_t *bi, gen_callable_t *gc, long gens_count) {
    bi_edsrm_info_t info;
    if (!bi_create_edsrm(bi, &info, DEFAULT_CACHE_SIZE)) {
        return false;
    }

    full_edsrm_t edsrm = {
        .info = &info,
        .gc = gc
    };

    gen_callable_t edsrm_gc = {
        .gen = (gen_t)full_edsrm_gen,
        .arg = &edsrm
    };

    print_hist(&edsrm_gc, 10, 30, gens_count);

    bi_edsrm_info_free(&info);

    return true;
}

bool bi_edsrm_misses(const bench_info_t *bi, gen_callable_t *gc, long gens_count, int *result) {
    bi_edsrm_info_t info;
    if (!bi_create_edsrm(bi, &info, DEFAULT_CACHE_SIZE)) {
        return false;
    }

    int res = 0;

    double temp;
    for (int i = 0; i < gens_count; i++) {
        while (!info.try_gen(&temp, gen_call(gc), gc, info.cache)) {
            res++;
        }
    }
    (*result) = res;

    bi_edsrm_info_free(&info);
    return true;
}

bool bi_run(const bench_info_t *bi, gen_callable_t *gc, bench_result_t *res, long gens_count) {
    return bi_idm_run(bi, gc, &res->idm, gens_count) && bi_edsrm_run(bi, gc, &res->edsrm, gens_count);
}