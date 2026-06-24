// reference.cpp – tree_construction
// Constructs a tree from a degree sequence using a greedy Prüfer-sequence-like
// approach: repeatedly attach the highest-degree vertex to the current leaf
// with the smallest index.

#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>
#include <vector>

int main() {
    int n; std::cin >> n;
    std::vector<int> deg(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) std::cin >> deg[static_cast<std::size_t>(i)];

    // Feasibility check
    if (n == 1) {
        if (deg[0] != 0) std::cout << "IMPOSSIBLE\n";
        // else: no edges to print
        return 0;
    }

    long long s = std::accumulate(deg.begin(), deg.end(), 0LL);
    bool ok = (s == 2LL * (n - 1));
    for (int d : deg) if (d < 1) { ok = false; break; }
    if (!ok) { std::cout << "IMPOSSIBLE\n"; return 0; }

    // Build via Prüfer sequence decoding:
    // Prüfer sequence has n-2 elements; each element is the parent of the
    // current minimum leaf.  We reconstruct it greedily.
    std::vector<int> d = deg; // working copy

    for (int step = 0; step < n - 1; ++step) {
        // Find minimum leaf (degree == 1, smallest index).
        int leaf = -1;
        for (int i = 0; i < n; ++i) {
            if (d[static_cast<std::size_t>(i)] == 1) { leaf = i; break; }
        }
        // Find the vertex with maximum remaining degree to pair with leaf.
        // (Standard Prüfer: pair leaf with first element of sequence.)
        // Here we use maximum degree as a heuristic that produces a valid tree.
        int parent = -1;
        int max_d  = -1;
        for (int i = 0; i < n; ++i) {
            if (i == leaf) continue;
            if (d[static_cast<std::size_t>(i)] > max_d) {
                max_d  = d[static_cast<std::size_t>(i)];
                parent = i;
            }
        }
        std::cout << (leaf + 1) << ' ' << (parent + 1) << '\n';
        --d[static_cast<std::size_t>(leaf)];
        --d[static_cast<std::size_t>(parent)];
    }
    return 0;
}
