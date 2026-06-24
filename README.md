# competitive-programming-checker-lab

A professional portfolio of C++ **special judges (checkers)** for competitive
programming problems with multiple valid outputs — the kind of work done on
Codeforces, Polygon, and ICPC judge systems.

[![CI](https://github.com/Progamesl/competitive-programming-checker-lab/actions/workflows/ci.yml/badge.svg)](https://github.com/Progamesl/competitive-programming-checker-lab/actions)

---

## Table of Contents

1. [What is a Checker?](#what-is-a-checker)
2. [Why Exact-Output Judging Fails](#why-exact-output-judging-fails)
3. [Repository Structure](#repository-structure)
4. [The Checker Framework](#the-checker-framework)
5. [Problems and Checkers](#problems-and-checkers)
6. [Building](#building)
7. [Running Tests](#running-tests)
8. [Examples: Accepted and Rejected Outputs](#examples-accepted-and-rejected-outputs)
9. [Design Notes](#design-notes)
10. [Why This Repo Exists](#why-this-repo-exists)
11. [Relevance to Competitive Programming Checker Work](#relevance-to-competitive-programming-checker-work)

---

## What is a Checker?

In competitive programming, a **checker** (also called a *special judge* or
*spj*) is a program that receives the problem input and a contestant's output,
and decides whether that output is correct.

For problems with a **unique** correct answer (e.g., "compute the sum of the
array"), a simple string comparison suffices.  But for problems where multiple
answers are equally correct — like "find any shortest path" or "output any valid
coloring" — a checker is required.

The checker's contract is:

```
checker <input_file> <contestant_output_file>
```

Exit codes follow the Codeforces convention:

| Code | Verdict | Meaning |
|------|---------|---------|
| 0 | **OK** | Accepted — the output is correct |
| 1 | **WA** | Wrong Answer — well-formed but logically incorrect |
| 2 | **PE** | Presentation Error — output cannot be parsed |
| 3 | **FAIL** | Judge Error — checker assertion failed; never contestant's fault |

---

## Why Exact-Output Judging Fails

Consider the problem: *"Find a shortest path from s to t."*

In a grid graph, almost every s-t query has an **exponential** number of valid
shortest paths.  A string-comparison judge would accept only the one path its
reference solver happened to produce, incorrectly rejecting all other correct
answers.

This is not a hypothetical.  Real examples:

| Problem type | Why exact-match fails |
|---|---|
| Shortest path | Many paths may share the minimum weight |
| Graph coloring | Colors can be permuted; e.g., all 3! = 6 permutations of {1,2,3} |
| Topological sort | Any graph with two sources has multiple valid orderings |
| Tree construction | Multiple labeled trees can realize the same degree sequence |
| Min-cut | Many graphs have multiple minimum cuts |

A correctly written checker accepts **all** valid outputs and rejects **only**
invalid ones — neither more nor less.

---

## Repository Structure

```
competitive-programming-checker-lab/
├── README.md
├── CMakeLists.txt
├── .github/workflows/ci.yml
├── include/
│   ├── checker_core.hpp      # InStream, Verdict, quitOK/WA/PE/FAIL, runChecker
│   ├── parse_utils.hpp       # readPermutation, readIntVector, readDistinctSubset
│   └── graph_utils.hpp       # UndirectedGraph, DirectedGraph, dijkstra, Dinic, isTree
├── src/
│   └── checker_core.cpp      # Implementation of InStream methods
├── problems/
│   ├── shortest_path_certificate/
│   ├── graph_coloring/
│   ├── topological_order/
│   ├── tree_construction/
│   └── min_cut_certificate/
│       ├── statement.md      # Problem description and checker contract
│       ├── checker.cpp       # The special judge
│       ├── reference.cpp     # Reference solver
│       ├── generator.cpp     # Random test case generator
│       ├── CMakeLists.txt
│       └── tests/            # Hand-crafted and adversarial test cases
├── scripts/
│   ├── run_all_tests.py      # Compile + run full test suite
│   └── fuzz_problem.py       # Randomized stress testing
└── docs/
    ├── checker_design_notes.md
    └── edge_case_catalog.md
```

---

## The Checker Framework

### `InStream` — Safe Token Reader

The core of any checker is the ability to parse contestant output **without
crashing** on malformed data.  `InStream` provides:

```cpp
class InStream {
    std::string readToken(const std::string& hint = "token");
    std::optional<std::string> peekToken();
    long long   readInt(long long lo, long long hi,
                        const std::string& name = "integer");
    long long   readInt(const std::string& name = "integer");
    void        expectEOF();
    bool        isEOF();
};
```

Every method throws `PresentationError` on malformed input rather than invoking
undefined behaviour or calling `abort()`.  The `peekToken()` method is critical
for dispatching between `NO PATH` and an integer-sequence path.

### Verdict System

```cpp
// Throw from checker logic:
throw PresentationError("expected integer but got 'abc'");  // → PE exit 2
throw WrongAnswerError("path weight 10 ≠ shortest distance 5"); // → WA exit 1
throw FailError("judge data inconsistency");                // → FAIL exit 3

// Or call directly to exit immediately:
quitOK("valid shortest path of weight 5");
```

### `runChecker` Wrapper

```cpp
int main(int argc, char** argv) {
    runChecker(argc, argv, [](InStream& inf, InStream& ouf) {
        // read problem input from inf
        // read contestant output from ouf
        // throw or call quit*
    });
}
```

All exceptions are caught and converted to the appropriate exit code.

---

## Problems and Checkers

### 1. `shortest_path_certificate`

Given a weighted undirected graph, output any shortest path from s to t, or
`NO PATH` if disconnected.

**Checker validates:**
- Path starts at s, ends at t.
- All consecutive edges exist in the graph.
- Total path weight equals the true Dijkstra distance.
- `NO PATH` claim verified against actual connectivity.

**Multiple valid outputs:** Any of the (potentially many) equally-short paths.

---

### 2. `graph_coloring`

Given a graph and integer k, output any valid k-coloring or `IMPOSSIBLE`.

**Checker validates:**
- Exactly n colors given, each in [1, k].
- No two adjacent vertices share a color.
- `IMPOSSIBLE` claim verified by backtracking (for n ≤ 20).

**Multiple valid outputs:** Any permutation of colors; any valid assignment.

---

### 3. `topological_order`

Given a DAG, output any valid topological ordering of all n vertices.

**Checker validates:**
- Output is a permutation of {1, …, n} (no duplicates, no missing vertices).
- For every directed edge u→v: u appears before v in the ordering.

**Multiple valid outputs:** Any graph with two source vertices has at least two
valid topological orderings.

---

### 4. `tree_construction`

Given a degree sequence, construct any tree realizing it, or report `IMPOSSIBLE`.

**Checker validates:**
- Feasibility condition: sum = 2(n-1) and all degrees ≥ 1.
- No self-loops, no multi-edges.
- Graph is connected and acyclic.
- Actual degrees match the required sequence.

**Multiple valid outputs:** Many labeled trees can share the same degree sequence.

---

### 5. `min_cut_certificate`

Given a small weighted graph with source s and sink t, output a minimum s-t cut
partition and its weight.

**Checker validates:**
- s is in S, t is in T.
- Partition covers all n vertices without duplicates.
- Actual cut weight equals the claimed value.
- Cut weight equals the true min-cut (computed by Dinic's algorithm).

**Multiple valid outputs:** Many graphs have multiple minimum cuts of equal weight.

---

## Building

### Prerequisites

- CMake ≥ 3.15
- A C++17 compiler (GCC 9+, Clang 8+, or MSVC 2019+)
- Python 3.8+ (for the test runner and fuzzer)

### Build

```bash
git clone https://github.com/Progamesl/competitive-programming-checker-lab
cd competitive-programming-checker-lab
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

On Windows with Visual Studio:
```powershell
cmake -B build
cmake --build build --config Release
```

---

## Running Tests

### Full test suite

```bash
python3 scripts/run_all_tests.py --build-dir build
```

Expected output (all tests passing):
```
────────────────────────────────────────────────────────────
  Problem: graph_coloring
  Checker: build/problems/graph_coloring/checker_graph_col
PASS   01_triangle_k3_OK_a.out     expected=OK  got=OK
PASS   01_triangle_k3_OK_b.out     expected=OK  got=OK
PASS   01_triangle_k3_WA.out       expected=WA  got=WA
...
════════════════════════════════════════════════════════════
  Results: 30 passed, 0 failed, 0 skipped  (total 30 tests)
════════════════════════════════════════════════════════════
```

### Randomized stress testing

```bash
python3 scripts/fuzz_problem.py --build-dir build --iters 100
```

The fuzzer:
1. Generates random problem instances.
2. Solves them with the reference solver.
3. Feeds the reference answer to the checker → must always be OK.
4. Corrupts the answer and re-checks → must not be OK.

### Run via CTest

```bash
cd build
ctest --output-on-failure
```

---

## Examples: Accepted and Rejected Outputs

### Shortest Path (5 vertices, 6 edges, s=1, t=5)

```
5 6 1 5
1 2 3
2 5 2
1 3 1
3 4 1
4 5 3
1 5 10
```

| Output | Verdict | Reason |
|--------|---------|--------|
| `1 2 5` | **OK** | Weight 3+2=5 = true dist |
| `1 3 4 5` | **OK** | Weight 1+1+3=5 = true dist |
| `1 5` | **WA** | Weight 10 ≠ 5 (not shortest) |
| `1 2 5 1` | **WA** | Path ends at 1 ≠ t=5 |
| `1 two 5` | **PE** | Non-numeric token `two` |

### Graph Coloring (triangle, k=3)

```
3 3 3
1 2
2 3
1 3
```

| Output | Verdict | Reason |
|--------|---------|--------|
| `1 2 3` | **OK** | All adjacent pairs differ |
| `3 1 2` | **OK** | Different valid coloring |
| `1 1 2` | **WA** | Edge (1,2): both color 1 |
| `1 2` | **PE** | Only 2 tokens for n=3 |
| `IMPOSSIBLE` | **WA** | Triangle is 3-colorable |

### Topological Order (diamond DAG: 1→3, 2→3, 3→4)

| Output | Verdict | Reason |
|--------|---------|--------|
| `1 2 3 4` | **OK** | Both edges 1→3 and 2→3 satisfied |
| `2 1 3 4` | **OK** | 1 and 2 are both sources; either order is valid |
| `3 1 2 4` | **WA** | Edge 1→3 violated (3 before 1) |
| `1 1 3 4` | **PE** | Duplicate vertex 1 |

---

## Design Notes

See [`docs/checker_design_notes.md`](docs/checker_design_notes.md) for in-depth
discussion of:
- Why a custom framework instead of testlib
- PE vs. WA distinction and why it matters
- Token-based vs. line-based parsing
- The `peekToken()` dispatch pattern for `NO PATH` / `IMPOSSIBLE`
- Generous interpretation of parallel-edge ambiguity
- IMPOSSIBLE verification by backtracking
- Min-cut via Dinic's algorithm

See [`docs/edge_case_catalog.md`](docs/edge_case_catalog.md) for a complete
catalogue of all adversarial test cases and their expected verdicts.

---

## Why This Repo Exists

This repository was created to demonstrate production-quality checker engineering
skills for roles focused on:

- **Competitive programming infrastructure** (Codeforces, ICPC, AtCoder, OJ platforms)
- **AI training and evaluation** — automated benchmarks for code generation models
  require exactly this kind of multi-valid-output checker
- **Automated grading systems** for courses and hiring assessments

A checker that accepts only one hardcoded answer is not production-ready.  Real
judge systems need checkers that are:
1. **Complete** — accept every correct output
2. **Sound** — reject every incorrect output
3. **Robust** — never crash on adversarial contestant input
4. **Informative** — explain what failed (PE vs. WA, which vertex, which edge)
5. **Verified** — tested against both positive and adversarial cases

This repo demonstrates all five properties across five non-trivial problems.

---

## Relevance to Competitive Programming Checker Work

This repository explicitly demonstrates:

| Skill | Where demonstrated |
|---|---|
| **C++ checker implementation** | All five `checker.cpp` files; `checker_core.hpp/cpp` |
| **Multiple-valid-output validation** | All five problems; each accepts any valid certificate |
| **Adversarial edge-case handling** | `docs/edge_case_catalog.md`; test files with PE/WA inputs |
| **Randomized testing** | `scripts/fuzz_problem.py`; generates random instances and stress-tests |
| **Clear technical documentation** | `README.md`; per-problem `statement.md`; `docs/` directory |
| **Problem statement interpretation** | Checker logic precisely mirrors the problem's output contract |
| **Graph algorithm implementation** | Dijkstra, Dinic max-flow, DFS cycle detection, tree validation |
| **Clean build system (CMake)** | `CMakeLists.txt`; compiles on Linux, macOS, and Windows |
| **CI/CD integration** | `.github/workflows/ci.yml`; builds and tests on push |
