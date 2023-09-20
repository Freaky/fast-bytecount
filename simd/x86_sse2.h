#if (defined(__i386__) || defined(__x86_64__)) && defined(WITH_SSE2)
#define HAVE_SSE2 1
#include <stdint.h>

size_t
sse2_bytecount_impl(uint8_t *haystack, const uint8_t n, size_t len);
#endif
