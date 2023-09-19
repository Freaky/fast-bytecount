
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

static size_t
_fallback_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	if (haystack_len < UINT32_MAX) {
		return naive_bytecount_32(haystack, needle, haystack_len);
	}

	return naive_bytecount(haystack, needle, haystack_len);
}

#ifdef WITH_AVX2
__attribute__((target("sse4.1,avx2")))
static size_t
_avx2_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	if (haystack_len >= 32) {
		return avx2_bytecount(haystack, needle, haystack_len);
	}

	#if defined(WITH_SSE4_1)
	if (haystack_len >= 16) {
		return sse41_bytecount(haystack, needle, haystack_len);
	}
	#endif

	return _fallback_bytecount(haystack, needle, haystack_len);
}
#endif

#ifdef WITH_SSE4_1
__attribute__((target("sse4.1")))
static size_t
_sse41_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	if (haystack_len >= 16) {
		return sse41_bytecount(haystack, needle, haystack_len);
	}

	return _fallback_bytecount(haystack, needle, haystack_len);
}
#endif

__attribute__((unused))
static size_t
(*resolve_bytecount (void))(uint8_t *, const uint8_t, size_t)
{
	__builtin_cpu_init();
	#ifdef WITH_AVX2
	if (__builtin_cpu_supports("avx2")) {
		return _avx2_bytecount;
	}
	#endif

	#ifdef WITH_SSE4_1
	if (__builtin_cpu_supports("sse4.1")) {
		return _sse41_bytecount;
	}
	#endif

	return _fallback_bytecount;
}

static size_t
inner_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len)
  __attribute__ (( ifunc ("resolve_bytecount") ));

size_t
bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	return inner_bytecount(haystack, needle, haystack_len);
}