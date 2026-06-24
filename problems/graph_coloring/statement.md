# Graph Coloring

## Problem Statement

Given an undirected graph with **n** vertices and **m** edges, and an integer
**k**, output any valid **k-coloring** of the vertices or `IMPOSSIBLE` if no
k-coloring exists.

A **k-coloring** assigns each vertex a color in `{1, 2, …, k}` such that no
two adjacent vertices share the same color.

### Input Format

```
n m k
u1 v1
u2 v2
...
um vm
```

* `1 ≤ n ≤ 100`
* `0 ≤ m ≤ n*(n-1)/2`
* `1 ≤ k ≤ n`
* No self-loops, no multi-edges.

### Output Format

**If a valid coloring exists:**
```
c1 c2 c3 ... cn
```
where `ci ∈ {1, …, k}` and no edge `(u, v)` has `cu == cv`.

**If impossible:**
```
IMPOSSIBLE
```

### Why Multiple Outputs Are Valid

For any graph with a valid k-coloring, there are typically many valid colorings
— different permutations of colors, or completely different color assignments —
all equally correct.  Even for a simple triangle with k=3 there are 6 valid
colorings (3! permutations of 3 colors).

### What the Checker Validates

**When a coloring is provided:**
1. Exactly `n` colors are given.
2. Every color is in `[1, k]`.
3. For every edge `(u, v)`: `color[u] ≠ color[v]`.

**When `IMPOSSIBLE` is claimed:**
4. Verified by brute-force backtracking (feasible for `n ≤ 20`).

### Edge Cases Covered

- Empty graph (`m = 0`): always colorable with k=1; `IMPOSSIBLE` is wrong.
- Complete graph `Kn`: requires exactly k = n colors.
- Single vertex: trivially 1-colorable.
- Bipartite graph: colorable with k=2; `IMPOSSIBLE` is wrong.
- `k = 1` with any edge: immediately `IMPOSSIBLE`.

### Examples

**Input (triangle, k=3):**
```
3 3 3
1 2
2 3
1 3
```
**Accepted outputs (all correct):**
```
1 2 3
```
```
3 1 2
```

**Input (triangle, k=2):**
```
3 3 2
1 2
2 3
1 3
```
**Correct output:**
```
IMPOSSIBLE
```
