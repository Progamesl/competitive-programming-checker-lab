// generator.cpp – min_cut_certificate
// Generates a random small weighted undirected graph.
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
    int      n        = (argc > 2) ? std::stoi(argv[2]) : 5;
    int      edge_pct = (argc > 3) ? std::stoi(argv[3]) : 50;

    n        = std::max(2, std::min(n, 20));
    edge_pct = std::max(10, std::min(edge_pct, 100));

    std::mt19937_64 rng(seed);
    auto randInt = [&](int lo, int hi) {
        return std::uniform_int_distribution<int>(lo, hi)(rng);
    };
    auto coin = [&]() { return randInt(1, 100) <= edge_pct; };

    std::set<std::pair<int,int>> used;
    std::vector<std::tuple<int,int,int>> edges;

    // Guarantee connectivity: random spanning tree.
    std::vector<int> perm(static_cast<std::size_t>(n));
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), rng);
    for (int i = 1; i < n; ++i) {
        int u = perm[static_cast<std::size_t>(i)];
        int v = perm[static_cast<std::size_t>(randInt(0, i - 1))];
        if (u > v) std::swap(u, v);
        edges.emplace_back(u, v, randInt(1, 20));
        used.insert({u, v});
    }
    // Extra random edges.
    for (int u = 0; u < n; ++u)
        for (int v = u + 1; v < n; ++v)
            if (!used.count({u, v}) && coin()) {
                edges.emplace_back(u, v, randInt(1, 20));
                used.insert({u, v});
            }

    int s = randInt(0, n - 2) + 1;
    int t;
    do { t = randInt(1, n); } while (t == s);

    std::cout << n << ' ' << static_cast<int>(edges.size())
              << ' ' << s << ' ' << t << '\n';
    for (auto& [u, v, w] : edges)
        std::cout << (u + 1) << ' ' << (v + 1) << ' ' << w << '\n';
    return 0;
}
