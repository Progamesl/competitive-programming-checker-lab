// generator.cpp – topological_order
// Generates a random DAG by creating edges only from lower-indexed to
// higher-indexed vertices (guaranteeing acyclicity), then shuffling vertex
// labels to obscure the trivial identity ordering.
// Usage: generator <seed> [n] [edge_prob_pct]

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    uint64_t seed     = (argc > 1) ? std::stoull(argv[1]) : 42;
    int      n        = (argc > 2) ? std::stoi(argv[2]) : 8;
    int      edge_pct = (argc > 3) ? std::stoi(argv[3]) : 30;

    n        = std::max(1, std::min(n, 200));
    edge_pct = std::max(0, std::min(edge_pct, 100));

    std::mt19937_64 rng(seed);
    auto coin = [&]() {
        return std::uniform_int_distribution<int>(1, 100)(rng) <= edge_pct;
    };

    // Randomly permute vertex labels so ordering isn't trivially 1..n.
    std::vector<int> label(static_cast<std::size_t>(n));
    std::iota(label.begin(), label.end(), 1);
    std::shuffle(label.begin(), label.end(), rng);

    std::set<std::pair<int,int>> used;
    std::vector<std::pair<int,int>> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (coin()) {
                // Edge from position i → position j in the topological order.
                int u = label[static_cast<std::size_t>(i)];
                int v = label[static_cast<std::size_t>(j)];
                if (!used.count({u,v})) {
                    edges.emplace_back(u, v);
                    used.insert({u,v});
                }
            }
        }
    }

    std::cout << n << ' ' << static_cast<int>(edges.size()) << '\n';
    for (auto& [u, v] : edges)
        std::cout << u << ' ' << v << '\n';
    return 0;
}
