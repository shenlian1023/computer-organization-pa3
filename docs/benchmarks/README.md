# PA3 benchmark record

The [archived terminal output](pa3-reported-run.txt) was supplied by the project author on 2026-09-29. This is the date of collection for the repository, not the date of the original run. The excerpt does not identify a source commit, compiler version, or Spike version.

## Reproduction on 2026-09-29

The repository's three assignment directories were copied to an isolated `/tmp` directory in the existing `asrlab/comp-org:pa3` container. Original coursework files were not changed. The blocked transpose source was synchronized from the local `CO2026PA3` version before this check.

The fresh run matched the archived record:

- Cache simulator: 3 PASS, 0 WA, 0 ERR.
- Transpose: 2 PASS, 0 WA, 0 ERR; improved misses 272 and 1,288.
- MLP: 1 PASS, 0 WA, 0 ERR; total overhead ratio `40.38469105393382`; maximum absolute output difference `7.629e-06`.

Commands within each assignment directory:

```bash
# 1_cachesim
make all
python3 judge.py 'input/*.in'

# 2_transpose
make all
make judge-all

# 3_mlp
make judge-all
```

These checks cover the bundled cases, not arbitrary inputs or hardware performance.

| Environment item | Reproduction value |
| --- | --- |
| Course image | `asrlab/comp-org:pa3` |
| RISC-V GCC | 14.2.0, build `g04696df096` |
| Spike | 1.1.1-dev |
| Valgrind | 3.22.0 |

Container image ID:

```text
sha256:7be375e48cd9e1e95a0836647dbc1f87db7ce5c305c12edde6cc9b4eed531de6
```

## Results in the record

| Check | Baseline | Improved | Interpretation |
| --- | ---: | ---: | --- |
| 32 × 32 transpose cache misses | 1,152 | 272 | 76.39% fewer misses; output passed |
| 64 × 64 transpose cache misses | 4,608 | 1,288 | 72.05% fewer misses; output passed |
| MLP instruction-cycle counter | 214,406,689 | 7,816,464 | Simulator counter, not hardware timing |
| MLP modeled memory overhead | 3,103,152,631 | 74,332,470 | Baseline/improved ratio: 41.75× |
| MLP modeled total overhead | 3,317,559,320 | 82,148,934 | Baseline/improved ratio: 40.38× |

The MLP output check passed with a maximum absolute difference of `7.629e-06`. The harness compares the improved output with the scalar reference using `abs(output - reference) <= 1e-3 + 1e-3 * abs(reference)`. It does not measure classification accuracy.

The record includes three cache traces, two transpose cases, and one MLP case. Reported scores are 60, 43.69, and 10 respectively; they are harness scores for these cases, not a verified final course grade.

## Cost model

The current [MLP judge](../../3_mlp/judge.py) uses this formula:

```text
memory_overhead = read_hits + write_hits + 100 * (read_misses + write_misses)
total_overhead  = memory_overhead + instruction_cycles
improvement    = baseline_total_overhead / improved_total_overhead
```

Substituting the archived values gives:

```text
(3,103,152,631 + 214,406,689) / (74,332,470 + 7,816,464)
= 40.38469105393382
```

This is a course-model comparison that adds an assumed memory cost to the simulator's instruction-cycle counter. It is not a silicon measurement, wall-clock speedup, or a cycle-accurate hardware model. Writeback counts appear in the log but are not a separate term in this formula.

## Reproduction and source alignment

The current harness compiles MLP code with `-O1 -static -march=rv64gcv`, runs Spike with `--isa=RV64GCV_Zicntr`, and defaults to a data cache with 16 sets, 4 ways, and 64-byte blocks. Both current baseline and improved configuration use that default. The excerpt alone does not confirm the historical run's versions or configuration.

The transpose evaluator uses Valgrind Lackey to collect memory-access traces, then the course cache simulator with 16 sets, 2 ways, and 32-byte blocks. Cache miss reduction is not a runtime speedup measurement.

The repository uses the local `CO2026PA3` four-row RVV implementation and the 8 × 8 blocked transpose synchronized on 2026-09-29. The archived log is not tied to a source commit; source alignment and a fresh reproduction are separate from those historical results.

Source fingerprints (SHA-256):

```text
2_transpose/snippet.c:
33c9e007cb2487eff79bf6b3ecc18c602b5822dfacb3afba38e517c9a190666a
3_mlp/matmul_improved.c:
c1d2a80a679c0500f05825bf6d33341eaf9029125e34b7552ef68047f38c5216
```

Before comparing new results, record the source commit, toolchain and simulator versions, exact commands, cache configuration, and input cases. Keep correctness output together with performance measurements.
