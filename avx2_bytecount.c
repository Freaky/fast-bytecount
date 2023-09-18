#include <stdint.h>
#include <immintrin.h>

#include "bytecount.h"

static const uint8_t MASK[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

static size_t
naive_bytecount(uint8_t *haystack, int n, size_t len) {
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

static __m256i
mm256_from_offset(const uint8_t* slice, size_t offset) {
    return _mm256_loadu_si256((__m256i*)(slice + offset));
}

static size_t
sum(const __m256i* u8s) {
    __m256i sums = _mm256_sad_epu8(*u8s, _mm256_setzero_si256());
    return _mm256_extract_epi64(sums, 0) + _mm256_extract_epi64(sums, 1) +
           _mm256_extract_epi64(sums, 2) + _mm256_extract_epi64(sums, 3);
}

size_t
bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
    if (haystack_len < 32) {
        return naive_bytecount(haystack, needle, haystack_len);
    }

    size_t offset = 0;
    size_t count = 0;

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
        count += sum(&counts);
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
        count += sum(&counts);
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
    count += sum(&counts);

    return count;
}


