
#include <stddef.h>
#include <stdint.h>

#include "bytecount.h"

#define BYTECOUNT_RUNTIME_DISPATCH (!WITHOUT_RUNTIME_DISPATCH && \
                                    HAVE_FUNC_ATTRIBUTE_IFUNC && \
                                    HAVE_FUNC_ATTRIBUTE_TARGET && \
                                    HAVE___BUILTIN_CPU_INIT && \
                                    HAVE___BUILTIN_CPU_SUPPORTS)
#define BYTECOUNT_32 (SIZEOF_SIZE_T == 8)

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

#if BYTECOUNT_32
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
#endif

static size_t
fallback_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
#if BYTECOUNT_32
	if (haystack_len < UINT32_MAX) {
		return naive_bytecount_32(haystack, needle, haystack_len);
	}
#endif
	return naive_bytecount(haystack, needle, haystack_len);
}

#if BYTECOUNT_RUNTIME_DISPATCH
# if __i386__ || __x86_64__
#  include "simd/x86_avx2.c"
#  include "simd/x86_sse2.c"
# endif

# if HAVE_AVX2
__attribute__((target("avx2")))
static size_t
avx2_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	if (haystack_len >= 32) {
		return avx2_bytecount_impl(haystack, needle, haystack_len);
	}

#  if defined(HAVE_SSE2)
	if (haystack_len >= 16) {
		return sse2_bytecount_impl(haystack, needle, haystack_len);
	}
#  endif

	return fallback_bytecount(haystack, needle, haystack_len);
}
# endif

# if HAVE_SSE2
__attribute__((target("sse2")))
static size_t
sse2_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	if (haystack_len >= 16) {
		return sse2_bytecount_impl(haystack, needle, haystack_len);
	}

	return fallback_bytecount(haystack, needle, haystack_len);
}
# endif

# if HAVE_FUNC_ATTRIBUTE_UNUSED
__attribute__((__unused__))
# endif
static size_t
(*resolve_bytecount (void))(uint8_t *, const uint8_t, size_t)
{
	__builtin_cpu_init();
# if HAVE_AVX2
	if (__builtin_cpu_supports("avx2")) {
		return avx2_bytecount;
	}
# endif

# if HAVE_SSE2
	if (__builtin_cpu_supports("sse2")) {
		return sse2_bytecount;
	}
# endif

	return fallback_bytecount;
}

static size_t
inner_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len)
    __attribute__((ifunc("resolve_bytecount")));

size_t
bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	return inner_bytecount(haystack, needle, haystack_len);
}
#else
size_t
bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	return fallback_bytecount(haystack, needle, haystack_len);
}
#endif


