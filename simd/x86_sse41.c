#ifdef WITH_SSE4_1
#include <assert.h>
#include <stdint.h>
#include <smmintrin.h>

static const uint8_t MASK[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

__attribute__((target("sse4.1")))
size_t
sse41_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len) {
    assert(haystack_len >= 16);

    #define mm_from_offset(haystack, offset) _mm_loadu_si128((__m128i *)(haystack + offset))
    #define SUM_ADD(count, u8s, temp) do {                                \
        temp = _mm_sad_epu8(u8s, _mm_setzero_si128());                   \
        count += _mm_extract_epi32(sums, 0) + _mm_extract_epi32(sums, 2); \
    } while (0)

	const uint8_t *ptr = (uint8_t *)haystack;
	size_t offset = 0;
	size_t count = 0;

	const __m128i needles = _mm_set1_epi8((uint8_t)needle);
    __m128i sums;

	// 4080
	while (haystack_len >= offset + 16 * 255) {
		__m128i counts = _mm_setzero_si128();

		for (size_t i=0; i < 255; i++) {
			counts = _mm_sub_epi8(
				counts,
				_mm_cmpeq_epi8(mm_from_offset(ptr, offset), needles)
			);
			offset += 16;
		}

        SUM_ADD(count, counts, sums);
	}

	// 2048
	if (haystack_len >= offset + 16 * 128) {
		__m128i counts = _mm_setzero_si128();

		for (size_t i=0; i < 128; i++) {
			counts = _mm_sub_epi8(
				counts,
				_mm_cmpeq_epi8(mm_from_offset(ptr, offset), needles)
			);
			offset += 16;
		}

		SUM_ADD(count, counts, sums);
	}

	// 16
	__m128i counts = _mm_setzero_si128();
	for (size_t i=0; i < ((haystack_len - offset) / 16); i++) {
		counts = _mm_sub_epi8(
			counts,
			_mm_cmpeq_epi8(mm_from_offset(ptr, offset + i * 16), needles)
		);
	}

	if (haystack_len % 16 != 0) {
		counts = _mm_sub_epi8(
			counts,
			_mm_and_si128(
				_mm_cmpeq_epi8(mm_from_offset(ptr, haystack_len - 16), needles),
				mm_from_offset(MASK, haystack_len % 16)
			)
		);
	}
    SUM_ADD(count, counts, sums);

	return count;
}
#endif