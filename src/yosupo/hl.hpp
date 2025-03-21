#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/flattenvector.hpp"

namespace yosupo {

struct HLEulerTour {
  public:
    int n, root;
    std::vector<int> ord, rord;

    explicit HLEulerTour(int _n, int _root = -1) : n(_n), root(_root) {
        edges.reserve(2 * (n - 1));
    }

    void add_edge(int u, int v) {
        edges.push_back({u, v});
        edges.push_back({v, u});
    }
    void set_root(int _root) { root = _root; }

    void build() {
        auto g = FlattenVector(n, edges);

        auto par = std::vector<int>(n, -1);
        auto topo = std::vector<int>();
        topo.reserve(n);
        topo.push_back(root);
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
        for (int i = n - 1; i >= 1; i--) {
            int u = topo[i];
            size[par[u]] += size[u];
            if (max_ch[par[u]] == -1 || size[u] > size[max_ch[par[u]]]) {
                max_ch[par[u]] = u;
            }
        }

        ord = std::vector<int>(n);
        rord.clear();
        rord.reserve(n);

        nxt = std::vector<int>(n);
        _size = std::vector<int>(n);

        std::vector<int> stack;
        stack.reserve(n);
        stack.push_back(root);

        while (stack.size()) {
            auto u = stack.back();
            stack.pop_back();

            int i = int(rord.size());
            ord[u] = i;
            rord.push_back(u);
            _size[i] = size[u];

            if (par[u] == -1 || max_ch[par[u]] != u) {
                nxt[i] = (par[u] == -1 ? -1 : ord[par[u]]) * 2;
            } else {
                nxt[i] = (nxt[i - 1] & 1) ? nxt[i - 1] : (i - 1) * 2 + 1;
            }

            for (int v : g.at(u)) {
                if (v == par[u] || v == max_ch[u]) continue;
                stack.push_back(v);
            }
            if (max_ch[u] != -1) {
                stack.push_back(max_ch[u]);
            }
        }
    }

    int _lca(int a, int b) const {
        if (a > b) std::swap(a, b);
        if (b < a + _size[a]) return a;
        while (a < b) {
            b = (nxt[b] >> 1);
        }
        return b;
    }
    int lca(int u, int v) const { return rord[_lca(ord[u], ord[v])]; }

    int subtree_size(int u) const { return _size[ord[u]]; }

  private:
    std::vector<std::pair<int, int>> edges;

    // key / value are ordinal
    std::vector<int> nxt, _size;
};

}  // namespace yosupo
