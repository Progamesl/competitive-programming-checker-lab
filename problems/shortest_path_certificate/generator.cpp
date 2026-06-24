// generator.cpp – shortest_path_certificate
// Generates a random weighted undirected graph and prints it to stdout.
// Usage: generator <seed> [n] [m_extra]
//
// Guarantees a connected graph by first building a random spanning tree,
// then adding m_extra random edges.

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    uint64_t seed    = (argc > 1) ? static_cast<uint64_t>(std::stoull(argv[1])) : 42;
    int      n       = (argc > 2) ? std::stoi(argv[2]) : 8;
    int      m_extra = (argc > 3) ? std::stoi(argv[3]) : 4;

    n = std::max(2, std::min(n, 50));
    m_extra = std::max(0, std::min(m_extra, n * (n - 1) / 2 - (n - 1)));

    std::mt19937_64 rng(seed);
    auto randInt = [&](int lo, int hi) {
        return std::uniform_int_distribution<int>(lo, hi)(rng);
    };

    // Build a random spanning tree (Prüfer sequence style).
    std::vector<int> perm(static_cast<std::size_t>(n));
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), rng);

    std::set<std::pair<int,int>> used;
    std::vector<std::tuple<int,int,int>> edges;

    for (int i = 1; i < n; ++i) {
        int u = perm[static_cast<std::size_t>(i)];
        int v = perm[static_cast<std::size_t>(randInt(0, i - 1))];
        if (u > v) std::swap(u, v);
        edges.emplace_back(u, v, randInt(1, 100));
        used.insert({u, v});
    }

    // Add random extra edges.
    int attempts = 0;
    int added    = 0;
    while (added < m_extra && attempts < 10000) {
        ++attempts;
        int u = randInt(0, n - 2);
        int v = randInt(u + 1, n - 1);
        if (!used.count({u, v})) {
            edges.emplace_back(u, v, randInt(1, 100));
            used.insert({u, v});
            ++added;
        }
    }

    int s = randInt(1, n);
    int t;
    do { t = randInt(1, n); } while (t == s);

    std::cout << n << ' ' << static_cast<int>(edges.size())
              << ' ' << s << ' ' << t << '\n';
    for (auto& [u, v, w] : edges)
        std::cout << (u + 1) << ' ' << (v + 1) << ' ' << w << '\n';

    return 0;
}
