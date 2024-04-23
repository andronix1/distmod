#include "benchs.h"

bool bi_edsrm_mnt(bench_info_t *bi, gen_callable_t *gc, double *res, long gens_count) {
    edsrm_mnt_pd_info_t pd_info = {
        .a = bi->a,
        .b = bi->b,
        .pd = bi->pd,
        .size = 330
    };
    edsrm_mnt_cfg_t cfg = {
        .pd_info = &pd_info,
        .prob_eq = dbd_prob_eq
    };

    edsrm_mnt_t cache;
    if (!edsrm_mnt_create(&cache, &cfg)) {
        return false;
    }

    MEASURE_TIME(*res, gens_count, {
        edsrm_mnt_generate(&cache, gc);
    });

    edsrm_mnt_free(&cache);

    return true;
}

bool bi_edsrm_2rng(bench_info_t *bi, gen_callable_t *gc, double *res, long gens_count) {
    edsrm_2rng_cfg_t cfg = {
        .a = bi->a,
        .b = bi->b,
        .pd = bi->pd,
        .lsize = 330,
        .rsize = 330,
        .prob_eq = dbd_prob_eq,
        .extremum = bi->extremum,
    };

    edsrm_2rng_t cache;
    if (!edsrm_2rng_create(&cache, &cfg)) {
        return false;
    }

    MEASURE_TIME(*res, gens_count, {
        edsrm_2rng_generate(&cache, gc);
    });

    edsrm_mnt_free(&cache);

    return true;
}

bool bi_idm(bench_info_t *bi, gen_callable_t *gc, double *res, long gens_count) {
    dfunc_t ipd = bi->ipd;
    MEASURE_TIME(*res, gens_count, {
        ipd(gen_call(gc));
    });
    return true;
}

bool bi_run(bench_info_t *bi, gen_callable_t *gc, bench_result_t *res, long gens_count) {
    return bi_idm(bi, gc, &res->idm, gens_count) && 
        (bi->type == BI_TYPE_MNT ? bi_edsrm_mnt : bi_edsrm_2rng)(bi, gc, &res->edsrm, gens_count);
}
