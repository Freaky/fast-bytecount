
AC_INIT([bytecount], [0.1], [tom@hur.st])
AC_PREREQ(2.67)

m4_include([build/ax_gcc_func_attribute.m4])
m4_include([build/ax_gcc_builtin.m4])
m4_include([build/ax_compile_check_sizeof.m4])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_PROG_CC

AC_ARG_ENABLE(runtime-dispatch,
	AS_HELP_STRING([--disable-runtime-dispatch], [disable runtime CPU feature detection]),
	[use_runtime_dispatch=$enableval])

AS_IF([test "$use_runtime_dispatch" = no], [
	AC_DEFINE(WITHOUT_RUNTIME_DISPATCH)
])

AC_ARG_ENABLE(runtime-sse2,
	AS_HELP_STRING([--disable-runtime-sse2], [disable runtime SSE2 acceleration]),
	[use_sse2=$enableval])

AS_IF([test "$use_sse2" = no], [
	AC_DEFINE(WITHOUT_SSE2)
])

AC_ARG_ENABLE(runtime-avx2,
	AS_HELP_STRING([--disable-runtime-avx2], [disable runtime AVX2 acceleration]),
	[use_avx2=$enableval])

AS_IF([test "$use_avx2" = no], [
	AC_DEFINE(WITHOUT_AVX2)
])

AC_CHECK_HEADERS([stdint.h])
AX_COMPILE_CHECK_SIZEOF(size_t)
AX_GCC_FUNC_ATTRIBUTE([unused])

AS_CASE([$target_alias], [*-*-*android*|*-*-*uclibc*|*-*-*musl*|*openbsd*], [true], [
	AX_GCC_FUNC_ATTRIBUTE([ifunc])
	AX_GCC_FUNC_ATTRIBUTE([target])
])

AX_GCC_BUILTIN([__builtin_cpu_init])
AX_GCC_BUILTIN([__builtin_cpu_supports])

AC_MSG_CHECKING(whether compiler understands -Wall -Wextra -Wpedantic)
old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall -Wextra -Wpedantic"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],
	AC_MSG_RESULT(yes),
	AC_MSG_RESULT(no)
	CFLAGS="$old_CFLAGS"
)

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
