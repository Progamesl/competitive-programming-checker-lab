// graph_utils.hpp
// Graph data structures and algorithms used across multiple problem checkers.
// Includes: weighted undirected and directed graphs, Dijkstra, Dinic max-flow,
// connectivity / acyclicity checks, and tree validation.

#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace checker {

static constexpr long long GRAPH_INF = std::numeric_limits<long long>::max() / 2;

// ─── Undirected weighted graph ────────────────────────────────────────────────

struct UndirectedGraph {
    int n; // vertices numbered 0 … n-1
    std::vector<std::vector<std::pair<int,long long>>> adj; // adj[u] = {(v, w)}
    std::vector<std::tuple<int,int,long long>>         edges; // original edge list

    explicit UndirectedGraph(int n_)
        : n(n_), adj(static_cast<std::size_t>(n_)) {}

    void addEdge(int u, int v, long long w) {
        adj[static_cast<std::size_t>(u)].emplace_back(v, w);
        adj[static_cast<std::size_t>(v)].emplace_back(u, w);
        edges.emplace_back(u, v, w);
    }

    bool hasEdge(int u, int v) const {
        for (auto& [nb, w] : adj[static_cast<std::size_t>(u)])
            if (nb == v) return true;
        return false;
    }
};

// ─── Directed graph ───────────────────────────────────────────────────────────

struct DirectedGraph {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<std::pair<int,int>> edges;

    explicit DirectedGraph(int n_)
        : n(n_), adj(static_cast<std::size_t>(n_)) {}

    void addEdge(int u, int v) {
        adj[static_cast<std::size_t>(u)].push_back(v);
        edges.emplace_back(u, v);
    }
};

// ─── Dijkstra ─────────────────────────────────────────────────────────────────

// Returns dist[v] = shortest distance from src, or GRAPH_INF if unreachable.
// All edge weights must be non-negative.
inline std::vector<long long> dijkstra(const UndirectedGraph& g, int src) {
    std::vector<long long> dist(static_cast<std::size_t>(g.n), GRAPH_INF);
    dist[static_cast<std::size_t>(src)] = 0;

    using P = std::pair<long long,int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0LL, src);

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[static_cast<std::size_t>(u)]) continue;
        for (auto& [v, w] : g.adj[static_cast<std::size_t>(u)]) {
            long long nd = dist[static_cast<std::size_t>(u)] + w;
            if (nd < dist[static_cast<std::size_t>(v)]) {
                dist[static_cast<std::size_t>(v)] = nd;
                pq.emplace(nd, v);
            }
        }
    }
    return dist;
}

// ─── Connectivity ─────────────────────────────────────────────────────────────

inline bool isConnected(const UndirectedGraph& g) {
    if (g.n == 0) return true;
    std::vector<bool> vis(static_cast<std::size_t>(g.n), false);
    std::queue<int> q;
    vis[0] = true; q.push(0);
    int cnt = 1;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto& [v, w] : g.adj[static_cast<std::size_t>(u)]) {
            if (!vis[static_cast<std::size_t>(v)]) {
                vis[static_cast<std::size_t>(v)] = true;
                ++cnt; q.push(v);
            }
        }
    }
    return cnt == g.n;
}

// ─── Acyclicity ───────────────────────────────────────────────────────────────

// True iff the undirected graph has no cycle.
// Handles multiple components; does NOT handle multi-edges (check separately).
inline bool isAcyclic(const UndirectedGraph& g) {
    std::vector<bool> vis(static_cast<std::size_t>(g.n), false);

    std::function<bool(int,int)> dfs = [&](int u, int par) -> bool {
        vis[static_cast<std::size_t>(u)] = true;
        for (auto& [v, w] : g.adj[static_cast<std::size_t>(u)]) {
            if (!vis[static_cast<std::size_t>(v)]) {
                if (!dfs(v, u)) return false;
            } else if (v != par) {
                return false; // back edge → cycle
            }
        }
        return true;
    };

    for (int i = 0; i < g.n; ++i)
        if (!vis[static_cast<std::size_t>(i)])
            if (!dfs(i, -1)) return false;
    return true;
}

// ─── Tree validation ──────────────────────────────────────────────────────────

inline bool isTree(const UndirectedGraph& g) {
    return static_cast<int>(g.edges.size()) == g.n - 1
        && isConnected(g)
        && isAcyclic(g);
}

// ─── Dinic max-flow ───────────────────────────────────────────────────────────

struct MaxFlow {
    struct Arc { int to, rev; long long cap; };

    int n;
    std::vector<std::vector<Arc>> g;
    std::vector<int> level, iter;

    explicit MaxFlow(int n_)
        : n(n_)
        , g(static_cast<std::size_t>(n_))
        , level(static_cast<std::size_t>(n_))
        , iter(static_cast<std::size_t>(n_)) {}

    void addEdge(int u, int v, long long cap) {
        g[static_cast<std::size_t>(u)].push_back(
            {v, static_cast<int>(g[static_cast<std::size_t>(v)].size()), cap});
        g[static_cast<std::size_t>(v)].push_back(
            {u, static_cast<int>(g[static_cast<std::size_t>(u)].size()) - 1, 0LL});
    }

    // Undirected: each direction holds full capacity.
    void addUndirectedEdge(int u, int v, long long cap) {
        g[static_cast<std::size_t>(u)].push_back(
            {v, static_cast<int>(g[static_cast<std::size_t>(v)].size()), cap});
        g[static_cast<std::size_t>(v)].push_back(
            {u, static_cast<int>(g[static_cast<std::size_t>(u)].size()) - 1, cap});
    }

    bool bfs(int s, int t) {
        std::fill(level.begin(), level.end(), -1);
        std::queue<int> q;
        level[static_cast<std::size_t>(s)] = 0;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (auto& e : g[static_cast<std::size_t>(v)]) {
                if (e.cap > 0 && level[static_cast<std::size_t>(e.to)] < 0) {
                    level[static_cast<std::size_t>(e.to)]
                        = level[static_cast<std::size_t>(v)] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[static_cast<std::size_t>(t)] >= 0;
    }

    long long dfs(int v, int t, long long pushed) {
        if (v == t) return pushed;
        for (int& i = iter[static_cast<std::size_t>(v)];
             i < static_cast<int>(g[static_cast<std::size_t>(v)].size()); ++i) {
            Arc& e = g[static_cast<std::size_t>(v)][static_cast<std::size_t>(i)];
            if (e.cap > 0
                && level[static_cast<std::size_t>(v)]
                       < level[static_cast<std::size_t>(e.to)]) {
                long long d = dfs(e.to, t, std::min(pushed, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    g[static_cast<std::size_t>(e.to)]
                     [static_cast<std::size_t>(e.rev)].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }

    long long maxflow(int s, int t) {
        long long flow = 0;
        while (bfs(s, t)) {
            std::fill(iter.begin(), iter.end(), 0);
            long long d;
            while ((d = dfs(s, t, GRAPH_INF)) > 0)
                flow += d;
        }
        return flow;
    }

    // Returns the source-side of the min-cut after maxflow() has been run.
    // A vertex is on the source side iff reachable from s in the residual graph.
    std::vector<bool> minCutSourceSide(int s) {
        std::vector<bool> vis(static_cast<std::size_t>(n), false);
        std::queue<int> q;
        vis[static_cast<std::size_t>(s)] = true;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (auto& e : g[static_cast<std::size_t>(v)]) {
                if (e.cap > 0 && !vis[static_cast<std::size_t>(e.to)]) {
                    vis[static_cast<std::size_t>(e.to)] = true;
                    q.push(e.to);
                }
            }
        }
        return vis;
    }
};

// Compute the minimum s-t cut value for an undirected weighted graph.
inline long long minCut(const UndirectedGraph& g, int s, int t) {
    MaxFlow mf(g.n);
    for (auto& [u, v, w] : g.edges)
        mf.addUndirectedEdge(u, v, w);
    return mf.maxflow(s, t);
}

} // namespace checker
