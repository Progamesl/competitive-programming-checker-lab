// checker.cpp – min_cut_certificate
//
// Accepts any minimum s-t cut partition with correct weight.
// Multiple minimum cuts may exist; the checker accepts all of them.
// The true minimum cut is computed internally using Dinic's max-flow.

#include "checker_core.hpp"
#include "graph_utils.hpp"

#include <numeric>
#include <unordered_set>
#include <vector>

using namespace checker;

int main(int argc, char** argv) {
    runChecker(argc, argv, [](InStream& inf, InStream& ouf) {
        // ── Read input ────────────────────────────────────────────────────
        int n = static_cast<int>(inf.readInt(2,  20,        "n"));
        int m = static_cast<int>(inf.readInt(0, 200,        "m"));
        int s = static_cast<int>(inf.readInt(1,   n,        "s")) - 1;
        int t = static_cast<int>(inf.readInt(1,   n,        "t")) - 1;

        UndirectedGraph g(n);
        for (int i = 0; i < m; ++i) {
            int u       = static_cast<int>(inf.readInt(1, n, "u")) - 1;
            int v       = static_cast<int>(inf.readInt(1, n, "v")) - 1;
            long long w = inf.readInt(1, 1'000'000LL, "w");
            g.addEdge(u, v, w);
        }

        // ── Compute true min-cut via Dinic's max-flow ─────────────────────
        long long true_min_cut = minCut(g, s, t);

        // ── Read contestant output ────────────────────────────────────────
        long long claimed_value = ouf.readInt(0,
            static_cast<long long>(m) * 1'000'000LL, "cut_value");

        int k = static_cast<int>(ouf.readInt(1, n - 1, "k"));

        // Read S partition.
        std::unordered_set<int> S_set;
        std::vector<int> S_vec;
        for (int i = 0; i < k; ++i) {
            int v = static_cast<int>(ouf.readInt(1, n, "S[" + std::to_string(i+1) + "]")) - 1;
            if (!S_set.insert(v).second)
                throw PresentationError(
                    "vertex " + std::to_string(v + 1)
                    + " appears more than once in S");
            S_vec.push_back(v);
        }
        ouf.expectEOF();

        // s must be in S.
        if (!S_set.count(s))
            throw WrongAnswerError(
                "source s=" + std::to_string(s + 1) + " is not in S");

        // t must NOT be in S (i.e., t is in T).
        if (S_set.count(t))
            throw WrongAnswerError(
                "sink t=" + std::to_string(t + 1) + " must be in T, not S");

        // Every vertex must appear exactly once across S ∪ T = {1..n}.
        // S has k distinct vertices in [1,n]; T = {1..n} \ S.
        // We've already checked no duplicates in S.  The implicit T = all other vertices.
        // Together they cover exactly n vertices.  Nothing more to check here.

        // Compute actual cut weight from the partition.
        std::vector<bool> in_S(static_cast<std::size_t>(n), false);
        for (int v : S_vec) in_S[static_cast<std::size_t>(v)] = true;

        long long actual_cut = 0;
        for (auto& [u, v, w] : g.edges) {
            if (in_S[static_cast<std::size_t>(u)]
                    != in_S[static_cast<std::size_t>(v)])
                actual_cut += w;
        }

        // The claimed value must match the actual partition weight.
        if (actual_cut != claimed_value)
            throw WrongAnswerError(
                "claimed cut_value=" + std::to_string(claimed_value)
                + " but actual weight of submitted partition is "
                + std::to_string(actual_cut));

        // The partition must be a minimum cut.
        if (actual_cut != true_min_cut)
            throw WrongAnswerError(
                "cut weight " + std::to_string(actual_cut)
                + " is not minimum (true min-cut = "
                + std::to_string(true_min_cut) + ")");

        quitOK("valid minimum cut of weight " + std::to_string(actual_cut));
    });
}
