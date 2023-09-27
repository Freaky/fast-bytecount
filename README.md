# fast-bytecount

This is a port of the AVX2 and SSE2 bytecount functions from the Rust
[bytecount] crate to C, including runtime CPU feature detection.

It isn't yet set up as a library, just a trivial benchmark against the naive
implementation.

## Usage

```
$ aclocal
$ autoconf
$ automake --add-missing
$ ./configure
$ make
$ ./bench
```

Supported `configure` flags:

* `--disable-runtime-dispatch`, enables only the scalar fallback bytecounts
* `--disable-runtime-avx2`, disable AVX2 version
* `--disable-runtime-sse2`, disable SSE2 version

[bytecount]: https://github.com/llogiq/bytecount
