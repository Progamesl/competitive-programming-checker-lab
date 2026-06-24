// reference.cpp – graph_coloring
// Reference solver using greedy coloring (outputs a coloring or IMPOSSIBLE).
// For small n this always finds a coloring if one with <= k colors exists
// (greedy uses at most chromatic number colors on a suitable vertex ordering).
// NOTE: for a more robust reference, backtracking is used as fallback.

#include <functional>
#include <iostream>
#include <vector>

int main() {
    int n, m, k;
    std::cin >> n >> m >> k;

    std::vector<std::vector<int>> adj(static_cast<std::size_t>(n));
    for (int i = 0; i < m; ++i) {
        int u, v; std::cin >> u >> v; --u; --v;
        adj[static_cast<std::size_t>(u)].push_back(v);
        adj[static_cast<std::size_t>(v)].push_back(u);
    }

    // Backtracking k-coloring.
    std::vector<int> color(static_cast<std::size_t>(n), 0);

    std::function<bool(int)> solve = [&](int u) -> bool {
        if (u == n) return true;
        for (int c = 1; c <= k; ++c) {
            bool ok = true;
            for (int nb : adj[static_cast<std::size_t>(u)]) {
                if (color[static_cast<std::size_t>(nb)] == c) { ok = false; break; }
            }
            if (!ok) continue;
            color[static_cast<std::size_t>(u)] = c;
            if (solve(u + 1)) return true;
        }
        color[static_cast<std::size_t>(u)] = 0;
        return false;
    };

    if (!solve(0)) {
        std::cout << "IMPOSSIBLE\n";
    } else {
        for (int i = 0; i < n; ++i) {
            if (i > 0) std::cout << ' ';
            std::cout << color[static_cast<std::size_t>(i)];
        }
        std::cout << '\n';
    }
    return 0;
}
