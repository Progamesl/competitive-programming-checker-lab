// checker.cpp – graph_coloring
//
// Accepts any valid k-coloring of the input graph, or "IMPOSSIBLE" when no
// k-coloring exists.  The IMPOSSIBLE claim is verified by backtracking (fast
// enough for the small n in this problem's constraints).

#include "checker_core.hpp"
#include "parse_utils.hpp"

#include <functional>
#include <string>
#include <vector>

using namespace checker;

// ── Backtracking k-colorability check ────────────────────────────────────────
// Returns true iff the graph (given as adjacency lists) admits a k-coloring.
// Only called when the contestant says IMPOSSIBLE; complexity O(k^n) but n≤20
// in practice, and the early-exit pruning is very effective.
static bool isKColorable(
    const std::vector<std::vector<int>>& adj, int n, int k)
{
    std::vector<int> color(static_cast<std::size_t>(n), 0);

    std::function<bool(int)> solve = [&](int u) -> bool {
        if (u == n) return true;
        for (int c = 1; c <= k; ++c) {
            bool ok = true;
            for (int nb : adj[static_cast<std::size_t>(u)]) {
                if (nb < u && color[static_cast<std::size_t>(nb)] == c) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
            color[static_cast<std::size_t>(u)] = c;
            if (solve(u + 1)) return true;
        }
        color[static_cast<std::size_t>(u)] = 0;
        return false;
    };
    return solve(0);
}

int main(int argc, char** argv) {
    runChecker(argc, argv, [](InStream& inf, InStream& ouf) {
        // ── Read input ────────────────────────────────────────────────────
        int n = static_cast<int>(inf.readInt(1, 100, "n"));
        int m = static_cast<int>(inf.readInt(0, n*(n-1)/2, "m"));
        int k = static_cast<int>(inf.readInt(1, n, "k"));

        std::vector<std::vector<int>> adj(static_cast<std::size_t>(n));
        // adj_matrix for fast edge lookup during coloring verification.
        std::vector<std::vector<bool>> edge(
            static_cast<std::size_t>(n),
            std::vector<bool>(static_cast<std::size_t>(n), false));

        for (int i = 0; i < m; ++i) {
            int u = static_cast<int>(inf.readInt(1, n, "u")) - 1;
            int v = static_cast<int>(inf.readInt(1, n, "v")) - 1;
            adj[static_cast<std::size_t>(u)].push_back(v);
            adj[static_cast<std::size_t>(v)].push_back(u);
            edge[static_cast<std::size_t>(u)][static_cast<std::size_t>(v)] = true;
            edge[static_cast<std::size_t>(v)][static_cast<std::size_t>(u)] = true;
        }

        // ── Read contestant output ────────────────────────────────────────
        auto first = ouf.peekToken();
        if (!first)
            throw PresentationError("output is empty");

        // ── Case 1: IMPOSSIBLE ────────────────────────────────────────────
        if (*first == "IMPOSSIBLE") {
            ouf.readToken();
            ouf.expectEOF();

            // Verify claim by brute-force (only safe for small n).
            if (n > 20)
                throw FailError(
                    "n=" + std::to_string(n)
                    + " is too large to verify IMPOSSIBLE by brute force");

            if (isKColorable(adj, n, k))
                throw WrongAnswerError(
                    "claimed IMPOSSIBLE but a valid " + std::to_string(k)
                    + "-coloring exists");

            quitOK("correctly reported IMPOSSIBLE");
        }

        // ── Case 2: coloring provided ─────────────────────────────────────
        std::vector<int> color;
        color.reserve(static_cast<std::size_t>(n));

        for (int i = 0; i < n; ++i)
            color.push_back(
                static_cast<int>(ouf.readInt(1, k, "color[" + std::to_string(i+1) + "]")));

        ouf.expectEOF();

        // Verify no monochromatic edge.
        for (int u = 0; u < n; ++u) {
            for (int v : adj[static_cast<std::size_t>(u)]) {
                if (v > u // check each undirected edge once
                    && color[static_cast<std::size_t>(u)]
                           == color[static_cast<std::size_t>(v)]) {
                    throw WrongAnswerError(
                        "edge (" + std::to_string(u+1) + ", "
                        + std::to_string(v+1) + ") has the same color "
                        + std::to_string(color[static_cast<std::size_t>(u)])
                        + " on both endpoints");
                }
            }
        }

        quitOK("valid " + std::to_string(k) + "-coloring");
    });
}
