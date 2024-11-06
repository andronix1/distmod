#pragma once

#include <distmod.h>
#include <math.h>

#define _STRINGIFY(v) #v
#define STRINGIFY(v) _STRINGIFY(v)

#define DEFAULT_M 500

typedef struct { 
	// pd -  probability distribution       f(x)
	// pdi - inverse probaility distibution f^(-1)(x) 
	// ipd - inversed probability function  F^(-1)(x)
	dfunc_t pd, ipd, pdi;
	double start, end;
} dist_t;

typedef struct {
	dist_t dist;
	const char *pd_str, *ipd_str, *pdi_str;
} full_dist_t;

edsrm_mnt_err_t dist_edsrm_init(edsrm_mnt_t *cache, dist_t *dist, size_t size) {
	edsrm_mnt_pd_info_t pd_info = {
		.pd = dist->pd,
		.a = dist->start,
		.b = dist->end,
		.size = size
	};
	edsrm_mnt_cfg_t cfg = {
		.pd_info = &pd_info,
		.prob_eq = dbd_prob_eq
	};
	return edsrm_mnt_init(cache, &cfg);
}

ziggurat_mnt_err_t dist_ziggurat_init(ziggurat_mnt_t *cache, dist_t *dist, size_t size) {
	ziggurat_mnt_config_t cfg = {
		.prob_eq = dbd_prob_eq,
		.ipd = dist->pdi,
		.pd = dist->pd,
		.start = dist->start,
		.end = dist->end,
		.size = size,
		// .use_ipd_for_gen = false
	};
	return ziggurat_mnt_init(cache, &cfg);
}

#define define_full_dist(name, pdc, pdic, ipdc, startv, endv) \
	double name##_pd(double u) { 		\
		return pdc;			\
	}					\
						\
	double name##_ipd(double u) { 		\
		return ipdc;			\
	}					\
						\
	double name##_pdi(double u) { 		\
		return pdic;			\
	}					\
						\
	full_dist_t name = {			\
		.pd_str = #pdc,			\
		.ipd_str = #ipdc,		\
		.pdi_str = #pdic,		\
		.dist = { 			\
			.pd = name##_pd,	\
			.ipd = name##_ipd,	\
			.pdi = name##_pdi,	\
			.start = startv, 	\
			.end = endv 		\
		} 				\
	};

#define pi 3.14159265359

define_full_dist(linear, 2.0*u, 0.5*u, 0, 0.0, 2.0);
define_full_dist(exponential, 1 / u, 1 / u, log(u), 0.0, 2.0);
define_full_dist(last, (pi + 1) * pow(u, pi), pow(u / (pi + 1), 1 / pi), pow(u, 1 / (pi + 1)), 0, 1);