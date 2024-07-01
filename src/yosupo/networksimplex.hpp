#pragma once

#include <algorithm>
#include <cmath>
#include <ranges>
#include <vector>

namespace yosupo {

template <class Cap, class Cost> struct NetworkSimplexGraph {
    struct Edge {
        int to;
        Cap cap;
        Cost cost;
    };
    std::vector<Edge> edges;
    std::vector<Cap> lowers;

    int n;
    std::vector<Cap> excess;

    explicit NetworkSimplexGraph(int _n = 0) : n(_n), excess(n) {}

    int add_vertex() {
        n++;
        return n - 1;
    }

    void add_excess(int i, Cap ex) { excess[i] += ex; }

    void add_edge(int u, int v, Cap lower, Cap upper, Cost cost) {
        edges.push_back({v, upper - lower, cost});
        edges.push_back({u, 0, -cost});
        excess[u] -= lower;
        excess[v] += lower;
        lowers.push_back(lower);
    }
};

template <class TotalCost, class Cap, class Cost> struct FlowResult {
    bool feasible;
    TotalCost cost;
    std::vector<Cap> flow;
    std::vector<Cost> potential;
};

template <class TotalCost, class Cap, class Cost>
FlowResult<TotalCost, Cap, Cost> solve(NetworkSimplexGraph<Cap, Cost> g) {
    auto edges = std::move(g.edges);
    auto excess = std::move(g.excess);
    int n = g.n;
    int m = int(edges.size());

    // potential
    std::vector<Cost> p(n + 1);

    Cost art_cost = 1;
    for (int i = 0; i < m; i += 2) {
        art_cost += std::abs(edges[i].cost);
    }

    struct Parent {
        int p;
        int e;  // edge id of (i -> p)
        Cap up, down;
    };
    std::vector<Parent> parents(n);

    edges.reserve(m + 2 * n);
    for (int i = 0; i < n; i++) {
        if (excess[i] >= 0) {
            edges.push_back({n, 0, art_cost});
            edges.push_back({i, excess[i], -art_cost});
            p[i] = -art_cost;
        } else {
            edges.push_back({n, -excess[i], -art_cost});
            edges.push_back({i, 0, art_cost});
            p[i] = art_cost;
        }
        parents[i].p = n;
        parents[i].e = int(edges.size()) - 2;
        parents[i].up = edges[parents[i].e].cap;
        parents[i].down = edges[parents[i].e ^ 1].cap;
    }

    std::vector<int> depth(n + 1, 1);
    depth[n] = 0;

    std::vector<int> next(2 * (n + 1)), prev(2 * (n + 1));
    auto conn = [&](int a, int b) {
        next[a] = b;
        prev[b] = a;
    };
    for (int i : std::views::iota(0, n + 1)) {
        conn(2 * i, 2 * i + 1);
    }
    for (int i : std::views::iota(0, n)) {
        conn(2 * i + 1, next[2 * n]);
        conn(2 * n, 2 * i);
    }

    auto push_flow = [&](int ei0) {
        const int u0 = edges[ei0 ^ 1].to, v0 = edges[ei0].to;
        const Cost cycle_len = edges[ei0].cost + p[u0] - p[v0];

        Cap flow = edges[ei0].cap;
        bool del_u_side = true;
        int del_u = v0;

        int lca = -1;
        {
            int u = u0, v = v0;
            auto up_u = [&]() {
                if (parents[u].down < flow) {
                    flow = parents[u].down;
                    del_u = u;
                    del_u_side = true;
                }
                u = parents[u].p;
            };
            auto up_v = [&]() {
                if (parents[v].up <= flow) {
                    flow = parents[v].up;
                    del_u = v;
                    del_u_side = false;
                }
                v = parents[v].p;
            };

            if (depth[u] >= depth[v]) {
                for (int i = 0; i < depth[u] - depth[v]; i++) {
                    up_u();
                }
            } else {
                for (int i = 0; i < depth[v] - depth[u]; i++) {
                    up_v();
                }
            }
            while (u != v) {
                up_u();
                up_v();
            }
            lca = u;
        }

        if (flow) {
            int u = u0;
            while (u != lca) {
                parents[u].up += flow;
                parents[u].down -= flow;
                u = parents[u].p;
            }
            int v = v0;
            while (v != lca) {
                parents[v].up -= flow;
                parents[v].down += flow;
                v = parents[v].p;
            }
        }

        int u = u0, par = v0;
        Cost p_diff = -cycle_len;
        std::pair<Cap, Cap> p_caps = {edges[ei0].cap - flow,
                                      edges[ei0 ^ 1].cap + flow};
        if (!del_u_side) {
            std::swap(u, par);
            std::swap(p_caps.first, p_caps.second);
            p_diff *= -1;
        }
        int par_e = ei0 ^ (del_u_side ? 0 : 1);

        while (par != del_u) {
            int d = depth[par];
            int idx = 2 * u;
            while (idx != 2 * u + 1) {
                if (idx % 2 == 0) {
                    d++;
                    p[idx / 2] += p_diff;
                    depth[idx / 2] = d;
                } else {
                    d--;
                }
                idx = next[idx];
            }

            conn(prev[2 * u], next[2 * u + 1]);
            conn(2 * u + 1, next[2 * par]);
            conn(2 * par, 2 * u);

            std::swap(parents[u].e, par_e);
            par_e ^= 1;

            std::swap(parents[u].up, p_caps.first);
            std::swap(parents[u].down, p_caps.second);
            std::swap(p_caps.first, p_caps.second);

            int next_u = parents[u].p;
            parents[u].p = par;
            par = u;
            u = next_u;
        }
        edges[par_e].cap = p_caps.first;
        edges[par_e ^ 1].cap = p_caps.second;
    };

    // pivot-rule: candidate-list
    const int LIST_LENGTH = std::max(int(0.2 * std::sqrt(double(m))), 10);
    const int MINOR_LIMIT = std::max(int(0.1 * LIST_LENGTH), 3);
    std::vector<int> candidates;
    candidates.reserve(LIST_LENGTH);

    auto minor = [&]() {
        if (candidates.empty()) return false;
        Cost best = 0;
        int best_ei = -1;

        int i = 0;
        while (i < int(candidates.size())) {
            int ei = candidates[i];
            if (!edges[ei].cap) {
                std::swap(candidates[i], candidates.back());
                candidates.pop_back();
                continue;
            }
            Cost c_len = edges[ei].cost + p[edges[ei ^ 1].to] - p[edges[ei].to];
            if (c_len >= 0) {
                std::swap(candidates[i], candidates.back());
                candidates.pop_back();
                continue;
            }
            if (c_len < best) {
                best = c_len;
                best_ei = ei;
            }
            i++;
        }

        if (best_ei == -1) return false;
        push_flow(best_ei);
        return true;
    };

    int ei = 0;
    while (true) {
        for (int i = 0; i < MINOR_LIMIT; i++) {
            if (!minor()) break;
        }

        Cost best = 0;
        int best_ei = -1;

        candidates.clear();
        for (int i = 0; i < int(edges.size()); i++) {
            if (edges[ei].cap) {
                Cost clen =
                    edges[ei].cost + p[edges[ei ^ 1].to] - p[edges[ei].to];
                if (clen < 0) {
                    if (clen < best) {
                        best = clen;
                        best_ei = ei;
                    }
                    candidates.push_back(ei);
                    if (int(candidates.size()) == LIST_LENGTH) break;
                }
            }

            ei++;
            if (ei == int(edges.size())) ei = 0;
        }
        if (candidates.empty()) break;
        push_flow(best_ei);
    }

    for (int i : std::views::iota(0, n)) {
        edges[parents[i].e].cap = parents[i].up;
        edges[parents[i].e ^ 1].cap = parents[i].down;
    }

    bool feasible = true;
    for (int i : std::views::iota(0, n)) {
        if (excess[i] >= 0) {
            if (edges[m + 2 * i + 1].cap) {
                feasible = false;
                break;
            }
        } else {
            if (edges[m + 2 * i].cap) {
                feasible = false;
                break;
            }
        }
    }
    if (!feasible) {
        return {false, {}, {}, {}};
    }

    TotalCost cost = 0;
    std::vector<Cap> flow;
    for (int i = 0; i < m; i += 2) {
        flow.push_back(g.lowers[i / 2] + edges[i ^ 1].cap);
        cost += TotalCost(1) * flow.back() * edges[i].cost;
    }
    p.pop_back();
    return {true, cost, flow, p};
}

}  // namespace yosupo
