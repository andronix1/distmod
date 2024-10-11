#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include <sched.h>
#include "distributions.h"

#define GENS_COUNT 10000000
#define measure_average_time(output, body) \
	do { 									\
		clock_t a = clock();						\
		for (size_t i = 0; i < GENS_COUNT; i++) body; 			\
		clock_t b = clock();						\
		output = (double)((b - a) * 1e9) / GENS_COUNT / CLOCKS_PER_SEC;	\
	} while (0)

multiplicative_rand_gen_t mul_rand_gen;

int main() {
    	cpu_set_t mask;
	CPU_ZERO(&mask);
    	CPU_SET(0, &mask);

    	if (sched_setaffinity(2, sizeof(cpu_set_t), &mask)) {
        	printf("[WARNING]: failed to set cpu affinity. it may cause less stable measurements!\n");
	}

	multiplicative_rand_gen_init(&mul_rand_gen);

	printf("measuring times...\n");

	// -----------------------
	edsrm_mnt_t edsrm;
	if (dist_edsrm_init(&edsrm, &linear.dist, DEFAULT_M)) {
		printf("failed to build edsrm\n");
		return 1;
	}
	double edsrm_time;
	measure_average_time(edsrm_time, edsrm_mnt_generate(&edsrm));
	printf("measured edsrm time\n");

	// -----------------------
	ziggurat_mnt_t ziggurat;
	if (dist_ziggurat_init(&ziggurat, &linear.dist, DEFAULT_M)) {
		printf("failed to build ziggurat\n");
		return 1;
	}

	double ziggurat_time;
	measure_average_time(ziggurat_time, ziggurat_mnt_generate(&ziggurat));
	printf("measured ziggurat time\n");

	// -----------------------
	double (*ipd)(double) = exponential.dist.ipd;
	double idfm_time;
	measure_average_time(idfm_time, ipd(rand_gen(multiplicative_rand_gen_generate(mul_rand_gen))));
	printf("measured ziggurat time\n");

	// -----------------------
	printf("edsrm: %f ns/gen\n", edsrm_time);
	printf("ziggurat: %f ns/gen\n", ziggurat_time);
	printf("idfm: %f ns/gen\n", idfm_time);
	return 0;
}
