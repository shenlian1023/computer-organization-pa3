# Cache simulation and RISC-V vector matrix computation

Three NCKU computer-organization exercises: track cache accesses with pseudo-LRU replacement, transpose matrices, and vectorize matrix multiplication used in MLP inference. The vector implementation processes four output rows together to reuse loaded values from matrix B.

`C` · `C++` · `PLRU` · `RISC-V Vector` · `Spike`

## Results and implementation at a glance

| Part | What this snapshot implements | Available evidence |
| --- | --- | --- |
| Cache simulation | Tree-based PLRU, valid/dirty tags, hit/miss and write-back accounting | Local course judge: **3 PASS, 0 WA, 0 ERR**, recorded on 2026-09-28 |
| Matrix transpose | Direct nested-loop transpose | [Implementation](2_transpose/snippet.c); not a cache-blocked version |
| MLP matrix multiplication | RVV floating-point multiply-accumulate, four-row blocking, and tail handling | [Vector implementation](3_mlp/matmul_improved.c) and [scalar baseline](3_mlp/matmul_naive.c) |

The cache result covers the three bundled traces on Ubuntu 22.04 under WSL. The RVV/MLP tests have not been rerun for this snapshot. A speedup ratio is not yet reported: it needs paired measurements under the same inputs, compiler flags, simulator configuration, and correctness tolerance.

## How the vector kernel works

```mermaid
flowchart LR
    A[Load a vector from matrix B] --> B[Reuse across four output rows]
    B --> C[RVV multiply-accumulate]
    C --> D[Store output vectors]
    D --> E[Process remaining output rows]
```

[matmul_improved.c](3_mlp/matmul_improved.c) uses RVV `e32m4` vectors. Processing four rows together reuses each loaded B vector across four accumulators, rather than treating each row as a separate pass. The code also handles rows left over after the four-row groups.

The assignment's [judge](3_mlp/judge.py) checks MLP output against reference values and uses simulator measurements for scoring. Those measurements are not hardware wall-clock timings.

## Cache simulator

[cachesim.cc](1_cachesim/cachesim.cc) reads cache configurations and memory-access traces. A PLRU tree chooses replacement candidates within each set; valid and dirty state determine misses and write-back behavior.

On Linux with `g++`, `make`, Python 3, and Git:

```bash
cd 1_cachesim
make all
python3 judge.py 'input/*.in'
```

Read the judge's PASS, WA, and ERR summary. Its exit status alone does not reliably indicate that every case passed.

## Run the remaining parts

The transpose harness is in [2_transpose](2_transpose). Its Makefile builds the drivers with `make all` and runs the bundled cases with `make judge-all`.

The MLP harness needs the course RISC-V environment: `riscv64-unknown-linux-gnu-gcc`, Spike, and the `RISCV` environment variable. It does not run as a normal x86 executable. The course container image is `asrlab/comp-org:pa3`.

```bash
# Inside the configured course RISC-V environment
cd 3_mlp
make judge-all
```

## Coursework scope

Each part retains its course Makefile and test harness. Test traces, answer files, and MLP inputs are provided course materials, not independently authored datasets. Student implementations and the framework coexist in this snapshot. No official assignment score is claimed.
