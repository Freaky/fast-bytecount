#if (defined(__i386__) || defined(__x86_64__)) && defined(WITH_AVX2)
#define HAVE_AVX2 1
#include <stdint.h>

size_t
avx2_bytecount(uint8_t *haystack, const uint8_t needle, size_t haystack_len);
#endif
