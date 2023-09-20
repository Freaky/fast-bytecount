# fast-bytecount

This is a port of the AVX2 and SSE2 bytecount functions from the Rust
[bytecount] crate to C, including runtime CPU feature detection (in theory).

[bytecount]: https://github.com/llogiq/bytecount
