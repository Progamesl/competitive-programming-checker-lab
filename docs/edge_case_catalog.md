# Edge Case Catalog

A systematic catalogue of adversarial inputs tested by the checker suite.
Each entry describes the input scenario, the expected verdict, and the
reasoning behind that verdict.

---

## shortest_path_certificate

| Test file | Scenario | Expected | Reason |
|---|---|---|---|
| `01_two_paths_OK_a.out` | Valid path `1 2 5`, weight 5 | OK | Correct shortest path |
| `01_two_paths_OK_b.out` | Valid path `1 3 4 5`, weight 5 | OK | Alternative shortest path of same weight |
| `01_two_paths_WA.out` | Path `1 5`, weight 10 | WA | Edge exists but weight 10 ≠ dist 5 |
| `01_two_paths_PE.out` | `1 two 5` | PE | Non-numeric token `two` |
| `02_disconnected_OK.out` | `NO PATH` | OK | Graph is disconnected |
| `02_disconnected_WA.out` | `1 2` | WA | Contestant gives a path but t=4 unreachable; path also doesn't reach t |
| `03_linear_OK.out` | `1 2 3 4` | OK | Unique shortest path |
| `03_linear_WA.out` | `1 3 4` | WA | Edge (1,3) does not exist in graph |
| `03_linear_PE.out` | `1 2 3 ABC` | PE | Non-numeric token `ABC` |
| `04_extra_tokens_PE.out` | `1 2 3 4 99` | PE | Vertex 99 out of range [1,4] |

**Notable ambiguities handled:**
- Two equally short paths accepted independently.
- `NO PATH` as two separate tokens, not one.
- Path ending early (not reaching t) is WA not PE.

---

## graph_coloring

| Test file | Scenario | Expected | Reason |
|---|---|---|---|
| `01_triangle_k3_OK_a.out` | `1 2 3` | OK | Valid 3-coloring of triangle |
| `01_triangle_k3_OK_b.out` | `3 1 2` | OK | Different valid 3-coloring |
| `01_triangle_k3_WA.out` | `1 1 2` | WA | Vertices 1 and 2 both color 1; edge (1,2) is monochromatic |
| `01_triangle_k3_PE.out` | `1 2` | PE | Only 2 colors for n=3 vertices → missing token |
| `02_triangle_k2_OK.out` | `IMPOSSIBLE` | OK | Triangle is not 2-colorable |
| `02_triangle_k2_WA.out` | `1 2 1` | WA | Edge (1,3) has colors 1,1 → monochromatic |
| `03_bipartite_k2_OK_a.out` | `1 2 1 2` | OK | Valid 2-coloring of path P4 |
| `03_bipartite_k2_OK_b.out` | `2 1 2 1` | OK | Swapped colors, equally valid |
| `04_empty_k1_OK.out` | `1 1 1 1` | OK | No edges → k=1 coloring is trivially valid |
| `04_empty_k1_WA.out` | `IMPOSSIBLE` | WA | Empty graph is always 1-colorable |

**Notable ambiguities handled:**
- `IMPOSSIBLE` is verified by backtracking, not by trusting the contestant.
- Empty graphs: never `IMPOSSIBLE` for any k ≥ 1.
- Color permutations (e.g., `{1,2,3}` vs `{3,1,2}`) all accepted.

---

## topological_order

| Test file | Scenario | Expected | Reason |
|---|---|---|---|
| `01_chain_OK.out` | `1 2 3 4` | OK | Only valid ordering for a chain |
| `01_chain_WA.out` | `1 3 2 4` | WA | Edge 2→3 violated: pos[2]=2 > pos[3]=1 |
| `01_chain_PE_dup.out` | `1 1 3 4` | PE | Duplicate vertex 1 |
| `02_diamond_OK_a.out` | `1 2 3 4` | OK | Valid topo order |
| `02_diamond_OK_b.out` | `2 1 3 4` | OK | Vertices 1 and 2 can be swapped |
| `02_diamond_WA.out` | `3 1 2 4` | WA | Vertex 3 before 1 but edge 1→3 exists |
| `03_disconnected_OK_a.out` | `1 2 3 4` | OK | Valid for disconnected DAG |
| `03_disconnected_OK_b.out` | `3 4 1 2` | OK | Components can appear in either order |
| `03_disconnected_PE_missing.out` | `1 2 3` | PE | n=4 but only 3 tokens; missing vertex → wrong range |

**Notable ambiguities handled:**
- Disconnected DAGs: any interleaving of components is valid.
- Sources with equal in-degree: free to appear in any relative order.

---

## tree_construction

| Test file | Scenario | Expected | Reason |
|---|---|---|---|
| `01_star_OK.out` | Star edges `1-2, 1-3, 1-4` | OK | Valid star realizing `[3,1,1,1]` |
| `01_star_WA_wrongdeg.out` | Triangle `2-3, 2-4, 3-4` | WA | Vertex 1 has degree 0 ≠ 3; also graph is a cycle, not a tree |
| `01_star_WA_selfloop.out` | Edge `1 1` present | WA | Self-loop not allowed in a tree |
| `02_path_OK_a.out` | `1-2, 2-3, 3-4` | OK | Valid path realizing `[1,2,2,1]` |
| `02_path_OK_b.out` | Same edges in different order | OK | Edge order is irrelevant |
| `03_impossible_OK.out` | `IMPOSSIBLE` | OK | Sum of degrees 4 ≠ 2*(4-1)=6 |
| `03_impossible_WA.out` | A tree given | WA | Degree sum is wrong; this tree has degrees `[1,2,2,1]` ≠ `[1,1,1,1]` |
| `04_impossible_zerodeg_OK.out` | `IMPOSSIBLE` | OK | Vertex 1 has degree 0 in n≥2 graph |

**Notable robustness checks:**
- Self-loops checked before connectivity/acyclicity to give a cleaner error message.
- Multi-edges detected via `std::set` before building the graph.
- Degree verification is per-vertex, pinpointing exactly which vertex is wrong.

---

## min_cut_certificate

| Test file | Scenario | Expected | Reason |
|---|---|---|---|
| `01_basic_OK_a.out` | `4 / 2 1 2` (S={1,2}) | OK | Min-cut weight 4, s∈S, t∉S |
| `01_basic_OK_b.out` | `4 / 2 2 1` (same S, different order) | OK | Vertex order within S is irrelevant |
| `01_basic_WA_suboptimal.out` | `5 / 1 1` (S={1}) | WA | Cut weight 5 > min-cut 4 |
| `01_basic_WA_t_in_S.out` | `4 / 3 1 2 4` (t=4 ∈ S) | WA | Sink must be in T |
| `01_basic_PE.out` | `four / 2 1 2` | PE | Non-numeric first token |
| `02_larger_OK.out` | `19 / 2 1 2` | OK | Min-cut of 5-vertex graph |

**Notable correctness checks:**
- Contestant's claimed value must equal the actual cut weight of their partition.
- The actual cut weight must equal the independently computed max-flow.
- Multiple valid cuts accepted (any partition achieving min weight).
- Vertex order within S is ignored (set semantics).
