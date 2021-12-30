#pragma once

#include <vector>
#include "atcoder/maxflow"

namespace yosupo {

template <class Cap, class Cost> struct CostScalingGraph {
    CostScalingGraph(int n) : _n(n), excess(n) {}
    struct Edge {
        int from, to;
        Cap lower_cap, upper_cap, flow;
        Cost cost;
    };

    void add_edge(int from, int to, Cap lower_cap, Cap upper_cap, Cost cost) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        assert(lower_cap <= upper_cap);
        max_cost = std::max(max_cost, cost);
        max_cost = std::max(max_cost, -cost);
        _edges.push_back({from, to, lower_cap, upper_cap, 0, cost});
    }

    void add_excess(int v, Cap x) { excess[v] += x; }

    std::pair<bool, Cost> solve() {
        for (auto e : _edges) {
            excess[e.to] += e.lower_cap;
            excess[e.from] -= e.lower_cap;
        }
        atcoder::mf_graph<Cap> mf_g(_n + 2);
        int sv = _n, tv = sv + 1;
        for (auto e : _edges) {
            mf_g.add_edge(e.from, e.to, e.upper_cap - e.lower_cap);
        }
        Cap excess_s_sum = 0, excess_t_sum = 0;
        for (int i = 0; i < _n; i++) {
            if (0 < excess[i]) {
                excess_s_sum += excess[i];
                mf_g.add_edge(sv, i, excess[i]);
            } else if (excess[i] < 0) {
                excess_t_sum += -excess[i];
                mf_g.add_edge(i, tv, -excess[i]);
            }
        }
        if (excess_s_sum != excess_t_sum || mf_g.flow(sv, tv) != excess_s_sum) {
            return {false, Cost(0)};
        }

        std::vector<std::vector<_Edge>> g(_n);
        std::vector<std::pair<int, int>> pos;
        int m = int(_edges.size());
        for (int i = 0; i < m; i++) {
            auto e = mf_g.get_edge(i);
            Cost cost = _edges[i].cost * _n;
            int from_id = int(g[e.from].size());
            int to_id = int(g[e.to].size());
            if (e.from == e.to) to_id++;
            pos.push_back({e.from, from_id});
            g[e.from].push_back({e.to, to_id, e.cap - e.flow, cost});
            g[e.to].push_back({e.from, from_id, e.flow, -cost});
        }

        solve(g);

        Cost answer = 0;
        for (int i = 0; i < m; i++) {
            auto p = pos[i];
            auto& e = _edges[i];
            auto ge = g[p.first][p.second];
            auto flow = e.upper_cap - ge.cap;
            e.flow = flow;
            answer += flow * e.cost;
        }

        _dual = std::vector<Cost>(_n);

        while (true) {
            bool update = false;
            for (int i = 0; i < _n; i++) {
                for (auto e : g[i]) {
                    if (!e.cap) continue;
                    auto cost = _dual[i] + e.cost / _n;
                    if (cost < _dual[e.to]) {
                        _dual[e.to] = cost;
                        update = true;
                    }
                }
            }
            if (!update) break;
        }

        return {true, answer};
    }

    std::vector<Cost> dual() const {
        return _dual;
    }
    std::vector<Edge> edges() const {
        return _edges;
    }

  private:
    int _n;
    std::vector<Edge> _edges;
    std::vector<Cap> excess;
    std::vector<Cost> _dual;
    Cost max_cost = 0;

    struct _Edge {
        int to, rev;
        Cap cap;
        Cost cost;
    };

    void solve(std::vector<std::vector<_Edge>>& g) {
        std::vector<Cost> dual(_n);
        Cost eps = _n * max_cost + 1;
        auto refine = [&]() {
            std::vector<Cap> excess(_n);
            auto cost = [&](int from, const _Edge& e) {
                return e.cost + dual[from] - dual[e.to];
            };
            auto push = [&](int from, _Edge& e, Cap cap) {
                excess[from] -= cap;
                excess[e.to] += cap;
                g[e.to][e.rev].cap += cap;
                e.cap -= cap;
            };
            for (int u = 0; u < _n; u++) {
                for (auto& e : g[u]) {
                    if (e.cap && cost(u, e) < 0) {
                        push(u, e, e.cap);
                    }
                }
            }
            std::vector<bool> actives_on(_n);
            atcoder::internal::simple_queue<int> actives;
            std::vector<int> iter(_n);

            auto relabel = [&](auto u) {
                iter[u] = 0;
                Cost down = (_n + 1) * max_cost;
                for (auto e : g[u]) {
                    if (e.cap) {
                        down = std::min(down, eps + cost(u, e));
                    }
                }
                dual[u] -= down;
                actives.push(u);
                actives_on[u] = true;
            };

            for (int i = 0; i < _n; i++) {
                if (excess[i] > 0) {
                    actives.push(i);
                    actives_on[i] = true;
                }
            }

            while (actives.size()) {
                int u = actives.front();
                actives.pop();
                actives_on[u] = false;
                for (int& i = iter[u]; i < int(g[u].size()); i++) {
                    auto& e = g[u][i];
                    if (e.cap && cost(u, e) < 0) {
                        push(u, e, std::min(excess[u], e.cap));
                        if (!actives_on[e.to] && excess[e.to] > 0) {
                            actives_on[e.to] = true;
                            actives.push(e.to);
                        }
                        if (excess[u] == 0) {
                            break;
                        }
                    }
                }
                if (excess[u] > 0) {
                    relabel(u);
                }
            }

            eps = 0;
            for (int u = 0; u < _n; u++) {
                for (auto& e : g[u]) {
                    if (e.cap) {
                        eps = std::max(eps, -(e.cost + dual[u] - dual[e.to]));
                    }
                }
            }
        };

        while (eps > 1) {
            eps = std::max<Cost>(eps >> 2, 1);
            refine();
        }
    }
};

}
