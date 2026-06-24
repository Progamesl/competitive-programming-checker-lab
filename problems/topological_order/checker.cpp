// checker.cpp – topological_order
//
// Verifies that the contestant's output is a valid topological ordering of
// the given DAG.  Any correct topological order is accepted.

#include "checker_core.hpp"
#include "parse_utils.hpp"

#include <vector>

using namespace checker;

int main(int argc, char** argv) {
    runChecker(argc, argv, [](InStream& inf, InStream& ouf) {
        // ── Read input ────────────────────────────────────────────────────
        int n = static_cast<int>(inf.readInt(1, 100'000, "n"));
        int m = static_cast<int>(inf.readInt(0, 200'000, "m"));

        std::vector<std::pair<int,int>> edges;
        edges.reserve(static_cast<std::size_t>(m));
        for (int i = 0; i < m; ++i) {
            int u = static_cast<int>(inf.readInt(1, n, "u"));
            int v = static_cast<int>(inf.readInt(1, n, "v"));
            edges.emplace_back(u, v);
        }

        // ── Read contestant permutation ───────────────────────────────────
        // readPermutation already checks range and duplicates, throwing PE.
        std::vector<int> perm = readPermutation(ouf, n);
        ouf.expectEOF();

        // ── Build position map: pos[vertex] = 0-based index in perm ──────
        std::vector<int> pos(static_cast<std::size_t>(n + 1), 0);
        for (int i = 0; i < n; ++i)
            pos[static_cast<std::size_t>(perm[static_cast<std::size_t>(i)])] = i;

        // ── Verify ordering constraint for every edge ─────────────────────
        for (auto& [u, v] : edges) {
            if (pos[static_cast<std::size_t>(u)]
                    >= pos[static_cast<std::size_t>(v)]) {
                throw WrongAnswerError(
                    "edge " + std::to_string(u) + " → "
                    + std::to_string(v)
                    + " is violated: position of " + std::to_string(u) + " is "
                    + std::to_string(pos[static_cast<std::size_t>(u)] + 1)
                    + " but position of " + std::to_string(v) + " is "
                    + std::to_string(pos[static_cast<std::size_t>(v)] + 1));
            }
        }

        quitOK("valid topological order");
    });
}
