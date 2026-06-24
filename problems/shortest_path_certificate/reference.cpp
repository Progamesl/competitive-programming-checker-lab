// reference.cpp – shortest_path_certificate
// Reference solver: outputs a shortest path from s to t (or "NO PATH").
// Uses Dijkstra with parent tracking to reconstruct one shortest path.

#include "checker_core.hpp"
#include "graph_utils.hpp"

#include <iostream>
#include <vector>

using namespace checker;

int main() {
    int n, m, s, t;
    std::cin >> n >> m >> s >> t;
    --s; --t; // 0-indexed

    UndirectedGraph g(n);
    for (int i = 0; i < m; ++i) {
        int u, v; long long w;
        std::cin >> u >> v >> w;
        g.addEdge(u - 1, v - 1, w);
    }

    // Dijkstra with parent reconstruction.
    std::vector<long long> dist(static_cast<std::size_t>(n), GRAPH_INF);
    std::vector<int>       parent(static_cast<std::size_t>(n), -1);
    dist[static_cast<std::size_t>(s)] = 0;

    using P = std::pair<long long,int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0LL, s);

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[static_cast<std::size_t>(u)]) continue;
        for (auto& [v, w] : g.adj[static_cast<std::size_t>(u)]) {
            long long nd = dist[static_cast<std::size_t>(u)] + w;
            if (nd < dist[static_cast<std::size_t>(v)]) {
                dist[static_cast<std::size_t>(v)]   = nd;
                parent[static_cast<std::size_t>(v)] = u;
                pq.emplace(nd, v);
            }
        }
    }

    if (dist[static_cast<std::size_t>(t)] == GRAPH_INF) {
        std::cout << "NO PATH\n";
        return 0;
    }

    // Reconstruct path by following parents from t to s.
    std::vector<int> path;
    for (int v = t; v != -1; v = parent[static_cast<std::size_t>(v)])
        path.push_back(v);
    std::reverse(path.begin(), path.end());

    for (int i = 0; i < static_cast<int>(path.size()); ++i) {
        if (i > 0) std::cout << ' ';
        std::cout << path[static_cast<std::size_t>(i)] + 1; // back to 1-indexed
    }
    std::cout << '\n';
    return 0;
}
