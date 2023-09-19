PROG=bench
SRCS=bench.c bytecount.c simd/x86_avx2.c simd/x86_sse41.c

CFLAGS=	-O3 -msse4.1 -mavx2 -std=c99 -Wall -Wextra -DWITH_SSE4_1 -DWITH_AVX2

OBJ=${SRCS:.c=.o}

all: ${PROG}

%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $<

${PROG}: ${OBJ}
	${CC} ${CFLAGS} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJ}

