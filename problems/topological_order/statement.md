# Topological Order

## Problem Statement

Given a directed acyclic graph (DAG) with **n** vertices and **m** directed edges,
output any valid **topological ordering** of the vertices.

A topological ordering is a permutation `v1, v2, …, vn` of the vertices
`{1, …, n}` such that for every directed edge `u → v`, vertex `u` appears
**before** vertex `v` in the ordering.

### Input Format

```
n m
u1 v1
u2 v2
...
um vm
```

* `1 ≤ n ≤ 100 000`
* `0 ≤ m ≤ 200 000`
* Edges are directed: `ui → vi`.
* The graph is guaranteed to be a DAG (no directed cycles).
* `1 ≤ ui, vi ≤ n`

### Output Format

A permutation of `{1, …, n}` on a single line:

```
v1 v2 v3 ... vn
```

### Why Multiple Outputs Are Valid

Any DAG with more than one source (zero-in-degree vertex) admits multiple valid
topological orderings.  The number of valid orderings can be exponential in the
number of vertices.  A checker that does exact string matching would accept only
one of the many correct answers.

### What the Checker Validates

1. Exactly `n` integers are present.
2. Every integer is in `[1, n]`.
3. No integer appears twice (the output is a permutation of `{1, …, n}`).
4. For every directed edge `u → v`: the position of `u` in the output is strictly
   less than the position of `v`.
5. No extra tokens after the permutation.

### Examples

**Input (diamond DAG):**
```
4 3
1 3
2 3
3 4
```

**Accepted outputs (both valid):**
```
1 2 3 4
```
```
2 1 3 4
```

**Rejected (violates edge 1→3):**
```
3 1 2 4
```
