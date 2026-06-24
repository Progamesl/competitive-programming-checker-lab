// generator.cpp – tree_construction
// Generates a random tree, extracts its degree sequence, and prints the input.
// Usage: generator <seed> [n]

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

int main(int argc, char** argv) {
    uint64_t seed = (argc > 1) ? std::stoull(argv[1]) : 42;
    int      n    = (argc > 2) ? std::stoi(argv[2]) : 6;
    n = std::max(1, std::min(n, 50));

    std::mt19937_64 rng(seed);

    // Build a random labeled tree using the random attachment model.
    std::vector<int> deg(static_cast<std::size_t>(n), 0);
    if (n >= 2) {
        std::vector<int> perm(static_cast<std::size_t>(n));
        std::iota(perm.begin(), perm.end(), 0);
        std::shuffle(perm.begin(), perm.end(), rng);
        for (int i = 1; i < n; ++i) {
            int parent = std::uniform_int_distribution<int>(0, i - 1)(rng);
            ++deg[static_cast<std::size_t>(perm[static_cast<std::size_t>(i)])];
            ++deg[static_cast<std::size_t>(perm[static_cast<std::size_t>(parent)])];
        }
    }

    std::cout << n << '\n';
    for (int i = 0; i < n; ++i) {
        if (i > 0) std::cout << ' ';
        std::cout << deg[static_cast<std::size_t>(i)];
    }
    std::cout << '\n';
    return 0;
}
