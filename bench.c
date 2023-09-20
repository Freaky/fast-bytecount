
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bytecount.h"

#define TEST_FILENAME "sherlock-holmes-huge.txt"
#define TEST_FILESIZE 594933
#define LOOPS 512

static double
monotime_ms() {
	struct timespec now;
	if (clock_gettime(CLOCK_MONOTONIC, &now)) {
		exit(65);
	}
	return ((double)now.tv_sec * 1000) + ((double)now.tv_nsec / 1000000.0);
}

int main(void) {
	int fd = open(TEST_FILENAME, O_RDONLY);

	if (!fd) {
		return 1;
	}

	uint8_t *buf[TEST_FILESIZE];
	size_t len = (size_t)read(fd, &buf, TEST_FILESIZE);
	(void)close(fd);

	if (len != TEST_FILESIZE) {
		return 64;
	}

	printf("char\tfast\tnaive\tdifference\n");

	int fast_count = 0;
	int libc_count = 0;
	double fast_cum = 0;
	double libc_cum = 0;
	for (int i=0; i < 255; i++) {
		double start, end, fast_time, naive_time;
		int loop = LOOPS;

		printf("%d\t", i);

		start = monotime_ms();
		while (loop--) {
			fast_count += bytecount((uint8_t *)buf, (uint8_t)i, len);
		}
		end = monotime_ms();
		fast_time = end - start;
		fast_cum += fast_time;

		printf("%.3fms\t", fast_time);

		loop = LOOPS;
		start = monotime_ms();
		while (loop--) {
			libc_count += naive_bytecount((uint8_t *)buf, (uint8_t)i, len);
		}
		end = monotime_ms();
		naive_time = end - start;
		libc_cum += naive_time;

		printf("%.3fms\t", naive_time);

		if (fast_time < naive_time) {
			printf("%.2fx faster", naive_time / fast_time);
		} else {
			printf("%.2fx slower", fast_time / naive_time);
		}

		printf("\n");

		if (fast_count != libc_count) {
			printf("counts differ: fast=%d != libc=%d\n", fast_count, libc_count);
			return 66;
		}
	}

	printf("Cumulative time: fast=%.2f libc=%.2f\n", fast_cum, libc_cum);

	printf("counted %d / %d\n", fast_count, libc_count);

	return 0;
}
