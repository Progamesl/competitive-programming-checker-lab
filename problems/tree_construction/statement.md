# Tree Construction

## Problem Statement

Given a degree sequence `d1, d2, …, dn`, construct a tree on `n` labeled
vertices where vertex `i` has exactly degree `di`, or report `IMPOSSIBLE` if
no such tree exists.

### Input Format

```
n
d1 d2 d3 ... dn
```

* `1 ≤ n ≤ 1 000`
* `0 ≤ di ≤ n - 1`

### Output Format

**If a tree exists:**
```
u1 v1
u2 v2
...
u_{n-1} v_{n-1}
```
(n-1 edges, in any order, each edge on its own line)

**If impossible:**
```
IMPOSSIBLE
```

### Feasibility Condition

A degree sequence `d1, …, dn` is realizable as a tree if and only if:
- `n = 1` and `d1 = 0`, **or**
- `n ≥ 2`, `sum(d) = 2*(n-1)`, and every `di ≥ 1`.

The second condition comes from the fact that every tree on n≥2 vertices is
connected (so every vertex has degree ≥ 1) and has exactly n-1 edges
(so the sum of degrees = 2*(n-1)).

### Why Multiple Outputs Are Valid

For most degree sequences there are many non-isomorphic labeled trees
that realize the same degree sequence.  For example, degree sequence
`[2, 2, 1, 1]` can be realized by:
- `1-2, 2-3, 3-4` (path 1-2-3-4, assigning deg[1]=1,deg[2]=2,deg[3]=2,deg[4]=1),
  but many other labeled variants exist.

The checker does not care which valid tree the contestant outputs.

### What the Checker Validates

1. The output is `IMPOSSIBLE` only when the degree sequence is infeasible.
2. Exactly `n-1` edges are given.
3. All vertex ids are in `[1, n]`.
4. No self-loops (`u ≠ v`).
5. No multi-edges.
6. The graph is connected.
7. The graph is acyclic (together with 2 and 6, this confirms it is a tree).
8. The degree of each vertex matches the required sequence.

### Examples

**Star (n=4, degrees=[3,1,1,1]):**
Correct output:
```
1 2
1 3
1 4
```

**Path (n=4, degrees=[1,2,2,1]):**
Correct output (one of many valid labelings):
```
1 2
2 3
3 4
```
