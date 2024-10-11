#include <distrand.h>
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>

#include "distributions.h"

inline void print_row(FILE *stream, char c, size_t n) {
	for (size_t i = 0; i < n; i++) {
		putc(c, stream);
	}
}

inline size_t size_t_len(size_t value) {
	size_t result = 1;
	do {
		result++;
	} while (value /= 10);
	return result;
}

#define DOUBLE_PRINT_PRECISION 6
#define FORMAT_PRINT_DOUBLE "%.0" STRINGIFY(DOUBLE_PRINT_PRECISION) "f"

inline size_t double_len(double value, size_t precision) {
	return size_t_len((size_t)value) + DOUBLE_PRINT_PRECISION;
}

#include <time.h>

void print_hist_of(double(*func)(void), size_t gens_count, size_t cols_count, size_t height) {
	double *gens = malloc(sizeof(double) * gens_count);
	gens[0] = func();
	double max_gen = gens[0],
	       min_gen = gens[0];

	for (size_t i = 1; i < gens_count; i++) {
		double gen = func();
		if (gen > max_gen) {
			max_gen = gen;
		}
		if (gen < min_gen) {
			min_gen = gen;
		}
		gens[i] = gen;
	}
	size_t *cols = malloc(sizeof(size_t) * cols_count);
	size_t max_col = 0;
	for (size_t i = 0; i < gens_count; i++) {
		size_t cid = trunc((gens[i] - min_gen) / (max_gen - min_gen) * cols_count);
		size_t col = cols[cid]++;
		if (col > max_col) {
			max_col = col;
		}
	}

	size_t min_col = gens_count;
	for (size_t i = 0; i < cols_count; i++) {
		size_t col = cols[i];
		if (col < min_col) {
			min_col = col;
		}
	}

	size_t max_col_size = size_t_len(max_col);
	size_t min_col_size = size_t_len(min_col);
	
	size_t min_gen_size = double_len(min_gen, 2);
	size_t max_gen_size = double_len(max_gen, 2);

	bool min_col_printed = false;
	print_row(stdout, '-', cols_count + max_col_size);
	putc('\n', stdout);
	for (int row = height - 1; row >= 0; row--) {
		if (row == height - 1) {
			printf("%d ", max_col);
		} else if (!min_col_printed && (row <= height * min_col / max_col || row == 1)) {
			min_col_printed = true;
			print_row(stdout, ' ', max_col_size - min_col_size);
			printf("%d ", min_col);
		} else {
			print_row(stdout, ' ', max_col_size);
		}
		for (size_t col = 0; col < cols_count; col++) {
			putc(row <= height * cols[col] / max_col ? '*' : ' ', stdout);
		}
		putc('\n', stdout);
	}
	
	print_row(stdout, ' ', max_col_size);
	printf(FORMAT_PRINT_DOUBLE, min_gen);
	print_row(stdout, ' ', cols_count - min_gen_size - max_gen_size);
	printf(FORMAT_PRINT_DOUBLE, max_gen);
	putc('\n', stdout);
	
	print_row(stdout, '-', cols_count + max_col_size);
	putc('\n', stdout);
	
	free(gens);
}

#define DEFAULT_M 500

ziggurat_mnt_t *ziggurat;
edsrm_mnt_t edsrm;
multiplicative_rand_gen_t *mul_rand_gen;
gen_callable_t *rand_gen;

double ziggurat_test(void) { return ziggurat_mnt_generate(ziggurat); }
double edsrm_test(void) { return edsrm_mnt_generate(&edsrm); }

#include <sched.h>
int main() {
	mul_rand_gen = multiplicative_rand_gen_create();

	gen_callable_t mrg_gc = {
		.arg = mul_rand_gen,
		.gen = (gen_t)multiplicative_rand_gen_generate
	};
	rand_gen = &mrg_gc;

	ziggurat = dist_ziggurat_create(&linear.dist, DEFAULT_M);
	if (ziggurat == NULL) {
		printf("failed to create ziggurat!\n");
		return 1;
	}

	
	if (dist_edsrm_init(&edsrm, &linear.dist, DEFAULT_M)) {
		printf("failed to create edsrm!\n");
		return 1;
	}

	printf("ziggurat\n");
	print_hist_of(ziggurat_test, 1000000, 50, 15);
	printf("edsrm\n");
	print_hist_of(edsrm_test, 1000000, 50, 15);

	for (size_t i = 0; i < 10; i++) {
		{
			clock_t a = clock();
			for (size_t i = 1; i < 100000000; i++) {
				ziggurat_mnt_generate(ziggurat);
			}
			clock_t b = clock();
			printf("%d\n", (b - a) * 1000000 / CLOCKS_PER_SEC);
		}
		{
			clock_t a = clock();
			for (size_t i = 1; i < 100000000; i++) {
				edsrm_mnt_generate(&edsrm);
			}
			clock_t b = clock();
			printf("%d\n", (b - a) * 1000000 / CLOCKS_PER_SEC);
		}
		{
			clock_t a = clock();
			for (size_t i = 1; i < 100000000; i++) {
				volatile double a = pow(gen_call(rand_gen), 1.0/ 10);
			}
			clock_t b = clock();
			printf("%d\n", (b - a) * 1000000 / CLOCKS_PER_SEC);
		}
	}

	edsrm_mnt_free(&edsrm);
	ziggurat_mnt_free(ziggurat);
	multiplicative_rand_gen_free(mul_rand_gen);
	return 0;
}
