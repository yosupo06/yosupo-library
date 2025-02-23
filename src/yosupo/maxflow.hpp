#pragma once

#include <vector>
#include "atcoder/maxflow.hpp"

namespace yosupo {

template <class Cap> struct MFWithLowerCapGraph {
  public:
    MFWithLowerCapGraph() : _n(0) {}
    explicit MFWithLowerCapGraph(int n) : _n(n), excess(n) {}

    struct edge {
        int from, to;
        Cap lower_cap, upper_cap, flow;
    };

    int add_edge(int from, int to, Cap lower_cap, Cap upper_cap) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        assert(0 <= lower_cap && lower_cap <= upper_cap);
        int m = int(_edges.size());
        _edges.push_back({from, to, lower_cap, upper_cap, 0});
        return m;
    }

    edge get_edge(int i) { return _edges[i]; }
    std::vector<edge> edges() { return _edges; }

    Cap max_flow(int s, int t) {
        assert(0 <= s && s < _n);
        assert(0 <= t && t < _n);
        assert(s != t);

        atcoder::mf_graph<Cap> g(_n + 2);
        int m = int(_edges.size());

        for (auto e : _edges) {
            g.add_edge(e.from, e.to, e.upper_cap - e.lower_cap);
            excess[e.to] += e.lower_cap;
            excess[e.from] -= e.lower_cap;
        }

        int excess_sum = 0;
        for (int i = 0; i < _n; i++) {
            if (excess[i] > 0) {
                excess_sum += excess[i];
                g.add_edge(_n, i, excess[i]);
            } else if (excess[i] < 0) {
                g.add_edge(i, _n + 1, -excess[i]);
            }
        }

        int flow0 = g.flow(_n, _n + 1);
        assert(flow0 <= excess_sum);

        int id = g.add_edge(t, s, excess_sum - flow0);

        int flow1 = g.flow(_n, _n + 1);

        g.change_edge(id, 0, 0);

        if (flow0 + flow1 != excess_sum) {
            return -1;
        }

        int flow2 = g.flow(s, t);
        for (int i = 0; i < m; i++) {
            _edges[i].flow = g.get_edge(i).flow + _edges[i].lower_cap;
        }
        return flow1 + flow2;
    }

  private:
    int _n;
    std::vector<edge> _edges;
    std::vector<Cap> excess;
};

}  // namespace yosupo
