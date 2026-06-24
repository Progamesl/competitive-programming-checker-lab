// reference.cpp – topological_order
// Kahn's algorithm (BFS-based topological sort).

#include <iostream>
#include <queue>
#include <vector>

int main() {
    int n, m;
    std::cin >> n >> m;

    std::vector<std::vector<int>> adj(static_cast<std::size_t>(n));
    std::vector<int> indeg(static_cast<std::size_t>(n), 0);

    for (int i = 0; i < m; ++i) {
        int u, v; std::cin >> u >> v; --u; --v;
        adj[static_cast<std::size_t>(u)].push_back(v);
        ++indeg[static_cast<std::size_t>(v)];
    }

    std::queue<int> q;
    for (int i = 0; i < n; ++i)
        if (indeg[static_cast<std::size_t>(i)] == 0) q.push(i);

    bool first = true;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (!first) std::cout << ' ';
        first = false;
        std::cout << (u + 1);
        for (int v : adj[static_cast<std::size_t>(u)]) {
            if (--indeg[static_cast<std::size_t>(v)] == 0)
                q.push(v);
        }
    }
    std::cout << '\n';
    return 0;
}
