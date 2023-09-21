PROG=bench
SRCS=bench.c bytecount.c simd/x86_avx2.c simd/x86_sse2.c

CFLAGS=	-O3 -g -std=c99 -Wall -Wextra -pedantic

OBJ=${SRCS:.c=.o}

all: ${PROG}

%.o: %.c
	${CC} ${CFLAGS} -o $@ -c $<

${PROG}: ${OBJ}
	${CC} ${CFLAGS} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJ}

