// reference.cpp – min_cut_certificate
// Finds a minimum s-t cut using Dinic's max-flow and outputs the partition.

#include "checker_core.hpp"
#include "graph_utils.hpp"

#include <iostream>
#include <vector>

using namespace checker;

int main() {
    int n, m, s, t;
    std::cin >> n >> m >> s >> t;
    --s; --t;

    UndirectedGraph raw(n);
    for (int i = 0; i < m; ++i) {
        int u, v; long long w;
        std::cin >> u >> v >> w;
        raw.addEdge(u - 1, v - 1, w);
    }

    // Run max-flow to find min-cut.
    MaxFlow mf(n);
    for (auto& [u, v, w] : raw.edges)
        mf.addUndirectedEdge(u, v, w);

    long long flow = mf.maxflow(s, t);
    auto in_S = mf.minCutSourceSide(s);

    // Collect S vertices.
    std::vector<int> S;
    for (int i = 0; i < n; ++i)
        if (in_S[static_cast<std::size_t>(i)]) S.push_back(i + 1);

    std::cout << flow << '\n';
    std::cout << static_cast<int>(S.size());
    for (int v : S) std::cout << ' ' << v;
    std::cout << '\n';
    return 0;
}
