#pragma once

#include <algorithm>
#include <vector>
#include <cassert>

#include "yosupo/flattenvector.hpp"

namespace yosupo {

struct HLEulerTour {
    HLEulerTour(int _n) : n(_n) {}

    void add_edge(int u, int v) {
        edges.push_back({u, v});
        edges.push_back({v, u});
    }

    void build(int r = 0) {
        auto g = FlattenVector(n, edges);

        auto par = std::vector<int>(n, -1);
        auto topo = std::vector<int>();
        topo.reserve(n);
        topo.push_back(r);
        for (int i = 0; i < n; i++) {
            int u = topo[i];
            for (int v : g.at(u)) {
                if (v == par[u]) continue;
                par[v] = u;
                topo.push_back(v);
            }
        }
        assert(int(topo.size()) == n);

        auto max_ch = std::vector<int>(n, -1);
        std::vector<int> size(n, 1);
        for (int i = n - 1; i >= 0; i--) {
            int u = topo[i];
            int max_size = -1;
            for (int v : g.at(u)) {
                if (v == par[u]) continue;

                size[u] += size[v];
                if (max_size < size[v]) {
                    max_size = size[v];
                    max_ch[u] = v;
                }
            }
        }

        ord = std::vector<int>(n);
        rord.clear();
        rord.reserve(n);

        nxt = std::vector<int>(n);

        std::vector<std::pair<int, bool>> stack;
        stack.reserve(n);
        stack.push_back({r, true});
        while (stack.size()) {
            auto [u, proot] = stack.back();
            stack.pop_back();

            int i = int(rord.size());
            ord[u] = i;
            rord.push_back(u);

            if (proot) {
                nxt[i] = par[u] == -1 ? 2 * n : ord[par[u]] + n;
            } else {
                nxt[i] = nxt[i - 1] >= n ? i - 1 : nxt[i - 1];
            }

            for (int v : g.at(u)) {
                if (v == par[u] || v == max_ch[u]) continue;
                stack.push_back({v, true});
            }
            if (max_ch[u] != -1) {
                stack.push_back({max_ch[u], false});
            }
        }

        _size = std::vector<int>(n);
        for (int i = 0; i < n; i++) {
            _size[i] = size[rord[i]];
        }
    }

    int _lca(int a, int b) const {
        if (a > b) std::swap(a, b);
        if (b < a + _size[a]) return a;
        while (a < b) {
            b = nxt[b];
            if (b >= n) b -= n;
        }
        return b;
    }
    int lca(int u, int v) const {
        return rord[_lca(ord[u], ord[v])];
    }

    int size(int u) const { return _size[u]; }

  private:
    int n;
    std::vector<std::pair<int, int>> edges;

    // key / value is ordinal
    std::vector<int> nxt, _size;

  public:
    std::vector<int> ord, rord;
};

}  // namespace yosupo
