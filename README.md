# Computer organization PA3

A three-part programming assignment covering cache replacement, matrix transposition, and RISC-V vector matrix multiplication used in MLP inference.

`C` · `C++` · `PLRU` · `RISC-V Vector` · `Spike`

## Assignment map

| Part | Implementation | Starting point |
| --- | --- | --- |
| Cache simulator | Tree-based pseudo-LRU replacement, valid and dirty tags, and access statistics | [cachesim.cc](1_cachesim/cachesim.cc) |
| Matrix transpose | Direct row-by-row transpose baseline | [snippet.c](2_transpose/snippet.c) |
| Matrix multiplication | RVV floating-point operations with four-row blocking and tail handling | [matmul_improved.c](3_mlp/matmul_improved.c) |

The transpose in this snapshot uses a direct nested loop. It does not implement a cache-blocked transpose.

## Cache simulator

The simulator accepts a cache configuration and a memory-access trace. A PLRU tree tracks replacement choices within each set. The cache records hits, misses, and evictions, while dirty lines distinguish write-back behavior.

On Linux with `g++`, `make`, Python 3, and Git:

```bash
cd 1_cachesim
make all
python3 judge.py 'input/*.in'
```

The supplied judge compares output against the bundled course answers. Inspect its PASS, WA, and ERR summary: its exit status alone does not reliably indicate that every case passed.

## Vector matrix multiplication

The improved implementation uses RVV `e32m4` vectors and floating-point multiply-accumulate operations. It processes four output rows together to reuse loaded values from matrix B, then handles remaining rows separately. A [scalar implementation](3_mlp/matmul_naive.c) provides the baseline.

The MLP test harness expects the course RISC-V environment, including `riscv64-unknown-linux-gnu-gcc`, Spike, and the `RISCV` environment variable. It cannot run as a normal x86 executable. The course container image is `asrlab/comp-org:pa3`.

## Files and attribution

Each part retains its course Makefile and test harness. Test traces, answer files, and MLP inputs are course materials, not independently authored datasets. Student changes and the provided framework coexist in this snapshot.

No speedup ratio or official assignment score is claimed. A measured comparison requires the same simulator configuration, compiler flags, inputs, and correctness tolerance for both implementations.

## Local verification

On 2026-09-28, the cache simulator passed all three bundled traces on Ubuntu 22.04 under WSL: 3 PASS, 0 WA, and 0 ERR. This check covers the bundled traces only. The RVV/MLP simulator tests have not been rerun for this snapshot.
