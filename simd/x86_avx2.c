#if (defined(__i386__) || defined(__x86_64__)) && defined(WITH_AVX2)
#include <assert.h>
#include <stdint.h>
#include <immintrin.h>

#include "x86_avx2.h"

static const uint8_t MASK[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

__attribute__((target("avx2")))
size_t
avx2_bytecount_impl(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
	assert (haystack_len >= 32);

#define SUM_ADD(count, u8s, temp) do {                                                         \
	temp = _mm256_sad_epu8(u8s, _mm256_setzero_si256());                                       \
	count += (size_t) _mm256_extract_epi64(temp, 0) + (size_t) _mm256_extract_epi64(temp, 1) + \
	         (size_t) _mm256_extract_epi64(temp, 2) + (size_t) _mm256_extract_epi64(temp, 3);  \
} while(0)

#define mm256_from_offset(slice, offset) _mm256_loadu_si256((const void *)(slice + offset))

	size_t offset = 0;
	size_t count = 0;

	__m256i sums;
	__m256i needles = _mm256_set1_epi8((char)needle);

	// 8160
	while (haystack_len >= offset + 32 * 255) {
		__m256i counts = _mm256_setzero_si256();
		for (int i = 0; i < 255; ++i) {
			counts = _mm256_sub_epi8(
				counts,
				_mm256_cmpeq_epi8(mm256_from_offset(haystack, offset), needles)
			);
			offset += 32;
		}
		SUM_ADD(count, counts, sums);
	}

	// 4096
	if (haystack_len >= offset + 32 * 128) {
		__m256i counts = _mm256_setzero_si256();
		for (int i = 0; i < 128; ++i) {
			counts = _mm256_sub_epi8(
			    counts,
			    _mm256_cmpeq_epi8(mm256_from_offset(haystack, offset), needles)
			);
			offset += 32;
		}
		SUM_ADD(count, counts, sums);
	}

	// 32
	__m256i counts = _mm256_setzero_si256();
	for (size_t i = 0; i < (haystack_len - offset) / 32; ++i) {
		counts = _mm256_sub_epi8(
		    counts,
		    _mm256_cmpeq_epi8(mm256_from_offset(haystack, offset + i * 32), needles)
		);
	}
	if (haystack_len % 32 != 0) {
		counts = _mm256_sub_epi8(
		    counts,
		    _mm256_and_si256(
		        _mm256_cmpeq_epi8(mm256_from_offset(haystack, haystack_len - 32), needles),
		        mm256_from_offset(MASK, haystack_len % 32)
		    )
		);
	}
	SUM_ADD(count, counts, sums);

	return count;
}
#endif
