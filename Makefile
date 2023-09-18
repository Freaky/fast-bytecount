PROG=bench
SRCS=bench.c bytecount.c
SSE2_SRCS=bench.c sse2_bytecount.c
AVX2_SRCS=bench.c avx2_bytecount.c

CFLAGS=-O2 -msse4.2 -mavx2 -march=native -std=gnu99 -Wall -Wextra

OBJ=${SRCS:.c=.o}
SSE2OBJ=${SSE2_SRCS:.c=.o}
AVX2OBJ=${AVX2_SRCS:.c=.o}

all: ${PROG} sse2 avx2

sse2: ${PROG}.sse2

avx2: ${PROG}.avx2

${PROG}.sse2: ${SSE2OBJ}
	${CC} ${CFLAGS} -o $@ ${SSE2OBJ} ${LDFLAGS}

${PROG}.avx2: ${AVX2OBJ}
	${CC} ${CFLAGS} -o $@ ${AVX2OBJ} ${LDFLAGS}


${PROG}: ${OBJ}
	${CC} ${CFLAGS} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $<

clean:
	rm -f ${PROG} ${PROG}.sse2 ${OBJ} ${SSE2OBJ}

