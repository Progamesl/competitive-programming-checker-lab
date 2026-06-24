// checker.cpp – shortest_path_certificate
//
// Accepts any valid shortest path from s to t, or "NO PATH" when disconnected.
// The contestant may output any of the (potentially many) equally-optimal paths.

#include "checker_core.hpp"
#include "graph_utils.hpp"

#include <map>
#include <string>
#include <vector>

using namespace checker;

int main(int argc, char** argv) {
    runChecker(argc, argv, [](InStream& inf, InStream& ouf) {
        // ── Read problem input ────────────────────────────────────────────
        int n = static_cast<int>(inf.readInt(2, 100'000, "n"));
        int m = static_cast<int>(inf.readInt(0, 200'000, "m"));
        int s = static_cast<int>(inf.readInt(1, n, "s")) - 1; // convert to 0-indexed
        int t = static_cast<int>(inf.readInt(1, n, "t")) - 1;

        UndirectedGraph g(n);
        for (int i = 0; i < m; ++i) {
            int u  = static_cast<int>(inf.readInt(1, n, "u")) - 1;
            int v  = static_cast<int>(inf.readInt(1, n, "v")) - 1;
            long long w = inf.readInt(1, 1'000'000'000LL, "w");
            g.addEdge(u, v, w);
        }

        // ── Compute true shortest distances from s ────────────────────────
        auto dist      = dijkstra(g, s);
        long long true_dist = dist[static_cast<std::size_t>(t)];

        // ── Read contestant output ────────────────────────────────────────
        auto first = ouf.peekToken();
        if (!first)
            throw PresentationError("output is empty");

        // ── Case 1: contestant claims no path ─────────────────────────────
        if (*first == "NO") {
            ouf.readToken(); // consume "NO"
            std::string second = ouf.readToken("PATH");
            if (second != "PATH")
                throw PresentationError(
                    "expected 'NO PATH' but got 'NO " + second + "'");
            ouf.expectEOF();

            if (true_dist != GRAPH_INF)
                throw WrongAnswerError(
                    "claimed NO PATH but a path of length "
                    + std::to_string(true_dist) + " exists");
            quitOK("correctly reported NO PATH");
        }

        // ── Case 2: contestant provides a path ────────────────────────────
        // Build adjacency lookup: (u,v) → minimum edge weight.
        // Using the minimum weight is generous toward the contestant when
        // parallel edges exist (the problem forbids them, but be safe).
        std::map<std::pair<int,int>, long long> edge_w;
        for (auto& [u, v, w] : g.edges) {
            auto k1 = std::make_pair(u, v);
            auto k2 = std::make_pair(v, u);
            if (!edge_w.count(k1) || edge_w[k1] > w) edge_w[k1] = w;
            if (!edge_w.count(k2) || edge_w[k2] > w) edge_w[k2] = w;
        }

        std::vector<int> path;
        while (!ouf.isEOF()) {
            long long raw = ouf.readInt(1, n, "path vertex");
            path.push_back(static_cast<int>(raw) - 1); // 0-indexed
        }

        if (path.empty())
            throw PresentationError("path is empty");

        if (path.front() != s)
            throw WrongAnswerError(
                "path starts at vertex " + std::to_string(path.front() + 1)
                + " but s=" + std::to_string(s + 1));

        if (path.back() != t)
            throw WrongAnswerError(
                "path ends at vertex " + std::to_string(path.back() + 1)
                + " but t=" + std::to_string(t + 1));

        if (true_dist == GRAPH_INF)
            throw WrongAnswerError(
                "contestant gave a path but s and t are disconnected");

        // Verify each step and accumulate weight.
        long long total = 0;
        for (int i = 0; i + 1 < static_cast<int>(path.size()); ++i) {
            int u = path[static_cast<std::size_t>(i)];
            int v = path[static_cast<std::size_t>(i + 1)];
            auto it = edge_w.find({u, v});
            if (it == edge_w.end())
                throw WrongAnswerError(
                    "edge (" + std::to_string(u + 1) + ", "
                    + std::to_string(v + 1)
                    + ") does not exist in the graph");
            total += it->second;
        }

        if (total != true_dist)
            throw WrongAnswerError(
                "path weight " + std::to_string(total)
                + " != shortest distance " + std::to_string(true_dist));

        quitOK("valid shortest path of weight " + std::to_string(total)
               + " with " + std::to_string(path.size()) + " vertices");
    });
}
