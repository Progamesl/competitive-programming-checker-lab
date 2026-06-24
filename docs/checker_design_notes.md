# Checker Design Notes

This document explains the key design decisions in the `competitive-programming-checker-lab`
framework and how they map to real-world checker engineering concerns.

---

## 1. Why a Custom Framework Instead of testlib?

[testlib](https://github.com/MikeMirzayanov/testlib) is the industry standard for
Codeforces/Polygon checkers.  This repository intentionally avoids it to:

1. **Show understanding from first principles.**  Anyone can call `quitOK()` from
   testlib without understanding why `peekToken()` needs to handle the internal
   lookahead buffer.  Writing it from scratch demonstrates real expertise.
2. **Self-contained portability.**  No external dependency = no version mismatch,
   no submodule, compiles on any C++17 toolchain.
3. **Educational clarity.**  The framework is intentionally smaller than testlib,
   making every decision traceable.

The `InStream` class replicates the most critical properties of testlib's `InStream`:
- Token-based (whitespace layout is irrelevant).
- Never crashes on malformed input; always throws `PresentationError`.
- Single-token lookahead via `peekToken()` for conditional dispatch.

---

## 2. The Three-Verdict Distinction

### PE — Presentation Error
Output cannot be parsed.  Examples:
- A non-numeric token where an integer is expected.
- Missing tokens (EOF before all required values are read).
- Extra tokens after the expected end of output.
- Value out of the allowed range (e.g., vertex id 0 when 1-indexed).

**Why separate from WA?**  In competitions, PE vs. WA can have different score
penalties, and they guide the contestant differently.  Saying "your integer is
out of range" is more informative than a generic "wrong answer."

### WA — Wrong Answer
Output is well-formed but logically incorrect.  Examples:
- A path with total weight greater than the true shortest distance.
- A coloring where two adjacent vertices share a color.
- A topological ordering that violates an edge constraint.

### FAIL — Judge Error
The checker's own assertions failed, or test data is inconsistent.  Contestants
never receive FAIL.  It signals a judge-side bug that needs fixing.

---

## 3. Token-Based vs. Line-Based Parsing

Most checker bugs come from line-based parsing:
```cpp
// Fragile: breaks if contestant uses Windows line endings or extra blank lines.
std::getline(ouf, line);
```

Token-based parsing (`InStream::readToken`) handles:
- Unix / Windows / Mac line endings transparently.
- Trailing spaces or blank lines.
- Multiple answers on one line vs. one per line (common ambiguity).

The `expectEOF()` method catches extra tokens regardless of whitespace.

---

## 4. Handling the IMPOSSIBLE / NO PATH Pattern

Several problems have a two-case output: either a certificate or a special
sentinel word.  The checker uses `peekToken()` to dispatch:

```cpp
auto first = ouf.peekToken();
if (*first == "NO") {
    // consume "NO" and then expect "PATH"
    ...
    // verify disconnectedness
} else {
    // parse path
    ...
}
```

This avoids the classic mistake of trying to parse an integer from "NO", which
would throw a PE (incorrect) when the answer is legitimately "NO PATH".

---

## 5. Generous Interpretation of Multi-Edge Ambiguity

When a contestant outputs a vertex sequence for a shortest path, consecutive
vertices `(u, v)` must be connected by an edge.  If the graph has parallel
edges between `u` and `v`, the checker uses the **minimum weight** edge,
giving the contestant the benefit of the doubt.  This follows the principle:

> A checker should accept the output if *any* valid interpretation of it is correct.

---

## 6. IMPOSSIBLE Verification by Brute Force

For the `graph_coloring` checker, when the contestant claims `IMPOSSIBLE`, the
checker verifies this via backtracking.  Key points:

- Only invoked for `n ≤ 20`; the checker throws `FAIL` for larger inputs where
  brute force is infeasible.
- Early-exit pruning makes backtracking fast in practice (usually < 1 ms for n ≤ 15).
- Completely independent of the reference solver — no risk of inheriting bugs.

---

## 7. Min-Cut Verification via Dinic's Algorithm

The `min_cut_certificate` checker recomputes the true min-cut using Dinic's
algorithm with the undirected-edge model (each undirected edge `(u, v, w)` adds
two directed arcs of capacity `w` in each direction).  This is O(V² · E) and
correct even when multiple min-cuts exist — the checker accepts any partition
whose cut weight equals the max-flow value.

---

## 8. Edge Cases That Checkers Must Handle

| Scenario | Handling |
|---|---|
| Empty contestant output | `peekToken()` returns nullopt → PE |
| All-whitespace output | Same as empty after token scanning |
| Vertex id 0 (off by one) | `readInt(1, n, …)` → PE |
| Negative vertex id | `readInt(1, n, …)` → PE |
| 64-bit overflow | `stoll` throws `out_of_range` → PE |
| Self-loop in tree | Explicit self-loop check → WA |
| Multi-edge in tree | `std::set` dedup → WA |
| Extra token at end | `expectEOF()` → PE |
| s == t in shortest path | dist[t]=0, path is just "s" — accepted |
| n=1 tree | 0-edge tree, empty output is correct |
| k=1, graph has edges | Graph coloring IMPOSSIBLE, checker verifies |

---

## 9. What This Framework Does Not Handle

- **Floating-point output**: none of the five problems use real numbers.
- **Interactive problems**: the framework is offline only.
- **Partial scoring**: no point deduction beyond 0/1 verdict per test.
- **Spj with jury answer file**: checkers self-verify; no `answer` file parameter.

These are intentional simplifications that keep the codebase focused on the
core checker engineering challenges.
