#if (__i386__ || __x86_64__) && defined(WITH_SSE4_1)
#define HAVE_SSE4_1 1
#include <stdint.h>

size_t
sse41_bytecount(uint8_t *haystack, const uint8_t n, size_t len);
#endif
