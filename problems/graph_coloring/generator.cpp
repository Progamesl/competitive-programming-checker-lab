// generator.cpp – graph_coloring
// Usage: generator <seed> [n] [edge_prob_pct] [k]
// Generates a random graph and random k value, then prints the input.

#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    uint64_t seed         = (argc > 1) ? std::stoull(argv[1]) : 42;
    int      n            = (argc > 2) ? std::stoi(argv[2]) : 6;
    int      edge_pct     = (argc > 3) ? std::stoi(argv[3]) : 40; // % probability
    int      k            = (argc > 4) ? std::stoi(argv[4]) : 3;

    n        = std::max(1, std::min(n, 20));
    edge_pct = std::max(0, std::min(edge_pct, 100));
    k        = std::max(1, std::min(k, n));

    std::mt19937_64 rng(seed);
    auto coin = [&]() {
        return std::uniform_int_distribution<int>(1, 100)(rng) <= edge_pct;
    };

    std::vector<std::pair<int,int>> edges;
    for (int u = 0; u < n; ++u)
        for (int v = u + 1; v < n; ++v)
            if (coin()) edges.emplace_back(u + 1, v + 1);

    std::cout << n << ' ' << static_cast<int>(edges.size())
              << ' ' << k << '\n';
    for (auto& [u, v] : edges)
        std::cout << u << ' ' << v << '\n';
    return 0;
}
