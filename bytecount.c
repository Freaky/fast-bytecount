
#include <stddef.h>
#include <stdint.h>

#include "bytecount.h"

size_t
bytecount(uint8_t *haystack, const uint8_t n, size_t len) {
	size_t count = 0;
	uint8_t *ptr = haystack;
	const uint8_t *end_ptr = ptr + len;

	while (ptr < end_ptr) {
		if (*ptr == (uint8_t)n) {
			count++;
		}
		ptr++;
	}

	return count;
}

