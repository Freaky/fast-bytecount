
#include <stddef.h>
#include <stdint.h>

#include "bytecount.h"

#ifdef WITH_AVX2
#include "simd/x86_avx2.h"
#endif

#ifdef WITH_SSE4_1
#include "simd/x86_sse41.h"
#endif

size_t
naive_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	size_t count = 0;
	uint8_t *ptr = haystack;
	const uint8_t *end_ptr = ptr + haystack_len;

	while (ptr < end_ptr) {
		if (*ptr == needle) {
			count++;
		}
		ptr++;
	}

	return count;
}

static size_t
naive_bytecount_32(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	uint32_t count = 0;
	uint8_t *ptr = haystack;
	const uint8_t *end_ptr = ptr + haystack_len;

	while (ptr < end_ptr) {
		if (*ptr == needle) {
			count++;
		}
		ptr++;
	}

	return count;
}

__attribute__((target_clones("sse4.1,avx2,default")))
static size_t
inner_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	#if defined(WITH_AVX2) && defined(__AVX2__)
	if (haystack_len >= 32) {
		return avx2_bytecount(haystack, needle, haystack_len);
	}
	#endif

	#if defined(WITH_SSE4_1) && defined(__SSE4_1__)
	if (haystack_len >= 16) {
		return sse41_bytecount(haystack, needle, haystack_len);
	}
	#endif

	if (haystack_len < UINT32_MAX) {
		return naive_bytecount_32(haystack, needle, haystack_len);
	}

	return naive_bytecount(haystack, needle, haystack_len);
}

size_t
bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	return inner_bytecount(haystack, needle, haystack_len);
}