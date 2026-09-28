# Implementation notes

[Project overview](../README.md) · [Benchmark conditions](benchmarks/README.md)

## Reusing B across four output rows

In row-major storage, `B[k*N+y]` begins a contiguous slice of row k. The kernel vectorizes along output columns and groups four output rows around that same slice.

Source: [`matmul`, four-row loop](../3_mlp/matmul_improved.c).

```c
size_t vl=VL(N-y);
v_t b,v0=ZERO(vl), v1 = ZERO(vl), v2 = ZERO(vl), v3 = ZERO(vl);
for(int k=0;k<K;k++){
    b=LOAD(B+k*N+y,vl);
    v0=FAM(v0,a0[k],b,vl), v1 = FAM(v1, a1[k], b, vl);
    v2 = FAM(v2, a2[k], b, vl), v3 = FAM(v3, a3[k], b, vl);
}
STORE(c0+y,v0,vl), STORE(c1 + y, v1, vl);
STORE(c2 + y,v2,vl), STORE(c3 + y,v3,vl);
y+=vl;
```

`LOAD` and `STORE` wrap RVV 32-bit vector loads and stores. `FAM` wraps `__riscv_vfmacc_vf_f32m4`: each A scalar multiplies the shared B vector and accumulates into its output vector. Holding the four partial sums in registers avoids storing and reloading C at every k iteration.

The four-row grouping reduces repeated B loads compared with processing those rows independently. It also uses more vector registers. The source uses `e32m4`; increasing the row group without checking register pressure could introduce spills. The benchmark does not compare alternative group sizes.

Two boundaries need separate handling. `VL(N-y)` chooses the active column count, and the loop advances by the returned `vl`. A second loop handles rows left over after `x+3<M` becomes false. The arithmetic work remains proportional to M×K×N. Vector execution and reuse change the work's execution cost, not the matrix multiplication's asymptotic complexity.

## Transposing without repeatedly evicting useful lines

Source: [blocked transpose](../2_transpose/snippet.c).

An 8×8 tile limits the working region. The implementation treats diagonal tiles separately: it copies each source row into B, then swaps the staged off-diagonal pairs. For off-diagonal 32×32 tiles, it loads eight adjacent A values into scalar temporaries before writing the transposed positions in B.

The 64×64 path stages part of the tile in a temporary region of B. Its first four source rows include this placement:

```c
B[j][k] = t0, B[j + 1][k] = t1, B[j + 2][k] = t2, B[j + 3][k] = t3;
B[j][k + 4] = t4, B[j + 1][k + 4] = t5, B[j + 2][k + 4] = t6, B[j + 3][k + 4] = t7;
```

The second line temporarily places the upper-right source quadrant in the destination's upper-right region. The next loop retrieves those values and moves them to their transposed positions in the lower-left region. It then fills the destination's upper-right and lower-right regions from the remaining A rows. No separate heap buffer is required.

This access order addresses cache conflicts within the assignment's cache configuration. The reproduced miss counts support the combined implementation: 272 for 32×32 and 1,288 for 64×64. There is no separate measurement of diagonal handling versus staging. Tile bounds and the size-dependent branch target the bundled dimensions; this is not a general transpose routine for arbitrary N.

## Encoding replacement decisions in a PLRU tree

Source: [`update_tree_on_hit`, `select_victim_plru`, and `access`](../1_cachesim/cachesim.cc).

Each set stores W−1 direction bits for W ways. On a hit, the implementation walks from the touched leaf toward the root and points each bit toward the other subtree:

```cpp
size_t node = way + ways -1;
while(node != 0){
  size_t parent = (node-1)>>1;
  plru_bits[idx*(ways-1)+parent]=(node == parent*2+1)?1:0;
  node = parent;
}
```

Victim selection follows the bits from the root to a leaf and flips the visited directions. Power-of-two associativity is checked during initialization. Tree maintenance takes a path of logarithmic length, but tag lookup still scans the ways linearly. PLRU approximates recency; it does not maintain an exact LRU ordering.

The access path also distinguishes valid and dirty state. Replacing a valid dirty entry increments writebacks and sends a store to the next cache level when a miss handler exists. A miss then fetches the aligned block. The current victim path follows PLRU directly; it does not explicitly scan for an invalid way first.

## Technical skills practiced

| Skill | Evidence in this implementation |
| --- | --- |
| Cache-locality analysis | Tile bounds, diagonal handling, and temporary placement in B |
| RISC-V vector programming | Vector-scalar FMA, dynamic active length, and remaining-row handling |
| State representation | Per-set PLRU bits and valid/dirty cache tags |
| Performance evaluation | Correctness checks paired with cache counters and an explicit cost formula |

Course harnesses, traces, simulator infrastructure, and inputs remain credited as supplied materials. The 40.38× result is modeled total overhead for one MLP case, not measured hardware speedup. Source excerpts explain the combined snapshot; they do not establish independent authorship of every framework component.
