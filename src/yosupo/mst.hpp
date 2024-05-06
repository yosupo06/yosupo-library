#pragma once

#include <vector>
#include <utility>
#include <ranges>

#include "yosupo/util.hpp"
#include "atcoder/dsu.hpp"

namespace yosupo {

template <class C>
struct MST {
    MST(int _n) { n = _n; }
    void add_edge(int u, int v, C c) {
        edges.push_back({u, v, c});
    }

    std::pair<C, std::vector<int>> mst() {
        int m = int(size(edges));
        std::vector<int> idx = yosupo::to_vec(std::views::iota(0, m));
        std::ranges::sort(idx, [&](int l, int r) {
            return edges[l].c < edges[r].c;
        });
        atcoder::dsu uf(n);
        C sum = C(0);
        std::vector<int> ids;
        for (int i: idx) {
            auto e = edges[i];
            if (uf.same(e.u, e.v)) continue;
            uf.merge(e.u, e.v);
            ids.push_back(i);
            sum += e.c;
        }
        return {sum, ids};
    }

  private:
    int n;
    struct E {
        int u, v;
        C c;
    };
    std::vector<E> edges;
};

}