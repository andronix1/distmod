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

multiplicative_rand_gen_t *mul_rand_gen;

int main() {
    	cpu_set_t mask;
	CPU_ZERO(&mask);
    	CPU_SET(0, &mask);

    	if (sched_setaffinity(2, sizeof(cpu_set_t), &mask)) {
        	printf("[WARNING]: failed to set cpu affinity. it may cause less stable measurements!\n");
	}

	mul_rand_gen = multiplicative_rand_gen_create();

	printf("measuring times...\n");
	edsrm_mnt_t edsrm;
	if (dist_edsrm_init(&edsrm, &linear.dist, 500)) {
		printf("failed to build edsrm\n");
		return 1;
	}
	double edsrm_time;
	measure_average_time(edsrm_time, edsrm_mnt_generate(&edsrm));
	printf("measured edsrm time\n");

	ziggurat_mnt_t *ziggurat;
	if (!(ziggurat = dist_ziggurat_create(&linear.dist, 500))) {
		printf("failed to build ziggurat\n");
		return 1;
	}
	printf("measured ziggurat time\n");
	double ziggurat_time;
	measure_average_time(ziggurat_time, ziggurat_mnt_generate(ziggurat));

	printf("edsrm: %f ns/gen\n", edsrm_time);
	printf("ziggurat: %f ns/gen\n", ziggurat_time);
	return 0;
}
