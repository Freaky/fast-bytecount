
.ifdef SSE2
PROG=bench-sse2
CFLAGS+=-DNDEBUG -msse2
WARNS=3
SRCS=bench.c sse2_bytecount.c
.else
PROG=bench
WARNS=6
SRCS=bench.c bytecount.c
.endif
MAN=

.include <bsd.prog.mk>
