# Shortest Path Certificate

## Problem Statement

Given a weighted undirected graph with **n** vertices and **m** edges, and two
distinct vertices **s** and **t**, output **any** shortest path from **s** to **t**,
or `NO PATH` if **s** and **t** are in different connected components.

### Input Format

```
n m s t
u1 v1 w1
u2 v2 w2
...
um vm wm
```

* `2 ≤ n ≤ 100 000`
* `0 ≤ m ≤ 200 000`
* `1 ≤ s, t ≤ n`, `s ≠ t`
* `1 ≤ ui, vi ≤ n`, no self-loops, no multi-edges
* `1 ≤ wi ≤ 10^9`

### Output Format

**If a path exists:** print the sequence of vertex ids forming the shortest path,
separated by spaces:

```
v1 v2 v3 ... vk
```

* The first vertex must be **s** and the last must be **t**.
* The total edge weight must equal the true shortest-path distance.
* Consecutive vertices `(vi, vi+1)` must be connected by an edge in the graph.

**If no path exists:** print exactly:

```
NO PATH
```

### Why Multiple Outputs Are Valid

Most graphs have **many** shortest paths — different vertex sequences of
identical total weight.  For example, in a grid graph almost every s-t query has
an exponential number of length-optimal routes.  An exact-match judge would
incorrectly reject any answer that isn't the judge's arbitrarily chosen canonical
path.

### What the Checker Validates

1. Output is either `NO PATH` (two tokens) **or** a non-empty sequence of integers.
2. All vertex ids are in `[1, n]`.
3. The path starts at `s` and ends at `t`.
4. Every consecutive pair of vertices is connected by an edge in the input graph.
5. The sum of edge weights along the path equals the true Dijkstra distance.
6. The `NO PATH` claim is verified against the actual connectivity.
7. No extra tokens after the answer.

### Example

**Input:**
```
5 6 1 5
1 2 3
2 5 2
1 3 1
3 4 1
4 5 3
1 5 10
```

**Accepted outputs (both correct):**
```
1 2 5
```
```
1 3 4 5
```

**Rejected (wrong weight — not a shortest path):**
```
1 5
```
(edge weight 10, but shortest distance is 5)
