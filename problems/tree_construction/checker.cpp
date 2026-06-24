// checker.cpp – tree_construction
//
// Verifies a contestant-submitted tree against a required degree sequence.
// Any tree realizing the degree sequence is accepted.

#include "checker_core.hpp"
#include "graph_utils.hpp"

#include <algorithm>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace checker;

// Check whether a degree sequence is realizable as a tree.
static bool isFeasible(const std::vector<int>& deg) {
    int n = static_cast<int>(deg.size());
    if (n == 1) return deg[0] == 0;
    long long s = std::accumulate(deg.begin(), deg.end(), 0LL);
    if (s != 2LL * (n - 1)) return false;
    for (int d : deg) if (d < 1) return false;
    return true;
}

int main(int argc, char** argv) {
    runChecker(argc, argv, [](InStream& inf, InStream& ouf) {
        // ── Read input ────────────────────────────────────────────────────
        int n = static_cast<int>(inf.readInt(1, 1000, "n"));

        std::vector<int> req_deg(static_cast<std::size_t>(n));
        for (int i = 0; i < n; ++i)
            req_deg[static_cast<std::size_t>(i)] =
                static_cast<int>(inf.readInt(0, n - 1, "d[" + std::to_string(i+1) + "]"));

        bool feasible = isFeasible(req_deg);

        // ── Read contestant output ────────────────────────────────────────
        auto first = ouf.peekToken();
        if (!first)
            throw PresentationError("output is empty");

        // ── Case 1: IMPOSSIBLE ────────────────────────────────────────────
        if (*first == "IMPOSSIBLE") {
            ouf.readToken();
            ouf.expectEOF();
            if (feasible)
                throw WrongAnswerError(
                    "claimed IMPOSSIBLE but a valid tree exists");
            quitOK("correctly reported IMPOSSIBLE");
        }

        // ── Case 2: tree provided ─────────────────────────────────────────
        if (!feasible)
            throw WrongAnswerError(
                "degree sequence is infeasible but contestant did not output IMPOSSIBLE");

        // n=1 edge case: tree has 0 edges, output should be empty.
        if (n == 1) {
            ouf.expectEOF();
            quitOK("trivially valid single-vertex tree");
        }

        UndirectedGraph g(n);
        std::set<std::pair<int,int>> seen_edges;

        for (int i = 0; i < n - 1; ++i) {
            int u = static_cast<int>(ouf.readInt(1, n, "u")) - 1;
            int v = static_cast<int>(ouf.readInt(1, n, "v")) - 1;

            if (u == v)
                throw WrongAnswerError(
                    "self-loop on vertex " + std::to_string(u + 1));

            auto key = std::make_pair(std::min(u, v), std::max(u, v));
            if (!seen_edges.insert(key).second)
                throw WrongAnswerError(
                    "multi-edge between vertices "
                    + std::to_string(u + 1) + " and " + std::to_string(v + 1));

            g.addEdge(u, v, 1);
        }
        ouf.expectEOF();

        if (!isConnected(g))
            throw WrongAnswerError("submitted graph is not connected");

        if (!isAcyclic(g))
            throw WrongAnswerError("submitted graph contains a cycle");

        // Verify degrees.
        for (int i = 0; i < n; ++i) {
            int got = static_cast<int>(g.adj[static_cast<std::size_t>(i)].size());
            if (got != req_deg[static_cast<std::size_t>(i)])
                throw WrongAnswerError(
                    "vertex " + std::to_string(i + 1) + " has degree " + std::to_string(got)
                    + " but required degree is " + std::to_string(req_deg[static_cast<std::size_t>(i)]));
        }

        quitOK("valid tree with correct degree sequence");
    });
}
