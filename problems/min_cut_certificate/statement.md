# Min-Cut Certificate

## Problem Statement

Given a small weighted undirected graph with **n** vertices, **m** edges, a
source **s**, and a sink **t**, find a minimum s-t cut and certify it by
reporting the partition and its weight.

An **s-t cut** is a partition of vertices into two disjoint sets **S** and **T**
such that `s ∈ S` and `t ∈ T`.  The **cut weight** is the sum of weights of
edges with one endpoint in **S** and the other in **T**.

A **minimum cut** is an s-t cut with the smallest possible cut weight.

### Input Format

```
n m s t
u1 v1 w1
u2 v2 w2
...
um vm wm
```

* `2 ≤ n ≤ 20`
* `1 ≤ m ≤ 200`
* `1 ≤ s, t ≤ n`, `s ≠ t`
* `1 ≤ wi ≤ 10^6`

### Output Format

```
cut_value
k v1 v2 ... vk
```

* `cut_value`: the claimed minimum cut weight.
* `k`: the number of vertices in **S** (the source side).
* `v1, …, vk`: the vertices in **S** (in any order, 1-indexed).
* The remaining `n - k` vertices are implicitly in **T**.

Constraints: `s` must be in **S**, `t` must be in **T**, every vertex appears
exactly once across **S** and **T**.

### Why Multiple Outputs Are Valid

Many graphs have multiple minimum cuts (different partitions with equal minimum
weight).  For example, in a symmetric graph the mirror partition can have the
same weight.  Exact-match judging would wrongly reject valid alternative cuts.

### What the Checker Validates

1. `cut_value` is an integer.
2. `k` vertices are listed for **S**, all in `[1, n]`, no duplicates.
3. `s ∈ S` and `t ∉ S`.
4. All `n` vertices are covered: exactly `{1..n}` across S ∪ T.
5. The actual cut weight (computed from the partition) equals `cut_value`.
6. `cut_value` equals the true minimum cut (computed by Dinic's max-flow).
7. No extra tokens.

### Example

**Input:**
```
4 4 1 4
1 2 3
1 3 2
2 4 2
3 4 3
```

True minimum cut = **4** (S = {1, 2}).

**Correct outputs (both valid):**
```
4
2 1 2
```
```
4
2 2 1
```

**Wrong answer (cut value is 5, not minimum):**
```
5
1 1
```

**Wrong answer (t=4 placed in S):**
```
4
3 1 2 4
```
