#pragma once

#include <algorithm>
#include <initializer_list>
#include <vector>
#include <cassert>
#include <iostream>

#include "atcoder/internal_csr.hpp"
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

        pos = std::vector<int>(n);
        ipos.clear();
        ipos.reserve(n);
        info = std::vector<Info>(n);

        std::vector<std::pair<int, bool>> stack;
        stack.reserve(n);
        stack.push_back({r, true});
        while (stack.size()) {
            auto [u, proot] = stack.back();
            stack.pop_back();

            int i = int(ipos.size());
            pos[u] = i;
            ipos.push_back(u);

            if (proot) {
                info[i].proot = i;
                info[i].ppar = par[u] == -1 ? -1 : pos[par[u]];
            } else {
                info[i].proot = info[i - 1].proot;
                info[i].ppar = info[i - 1].ppar;
            }

            for (int v : g.at(u)) {
                if (v == par[u] || v == max_ch[u]) continue;
                stack.push_back({v, true});
            }
            if (max_ch[u] != -1) {
                stack.push_back({max_ch[u], false});
            }
        }
    }

    int lca(int u, int v) const {
        int a = pos[u], b = pos[v];
        while (a != b) {
            if (a > b) std::swap(a, b);
            if (info[b].proot <= a) {
                break;
            } else {
                b = info[b].ppar;
            }
        }
        return ipos[a];
    }

  private:
    int n;
    struct Edge {
        int to;
    };
    std::vector<std::pair<int, int>> edges;

  public:
    std::vector<int> ord, rord, pos, ipos;
    struct Info {
        int proot = -1; // path root
        int ppar; //path parent
    };
    std::vector<Info> info;
};
std::ostream& operator<<(std::ostream& os, HLEulerTour::Info i) {
    return os << "Info(" << i.proot << ", " << i.ppar << ")";
}

}  // namespace yosupo
