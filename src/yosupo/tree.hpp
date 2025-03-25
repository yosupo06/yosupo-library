#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/flattenvector.hpp"

namespace yosupo {

struct RootedTree {
    int n, root;
    FlattenVector<int> children;
    std::vector<int> topo, par;
};

struct RootedTreeBuilder {
    RootedTreeBuilder(int _n) : n(_n) { edges.reserve(2 * (n - 1)); }

    void add_edge(int u, int v) {
        edges.push_back({u, v});
        edges.push_back({v, u});
    }

    RootedTree build(int root) && {
        assert(int(edges.size()) == 2 * (n - 1));

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

        g.erase_if([&](const std::pair<int, int>& edge) {
            return par[edge.second] != edge.first;
        });
        return {n, root, std::move(g), std::move(topo), std::move(par)};
    }

  private:
    int n;
    std::vector<std::pair<int, int>> edges;
};

}  // namespace yosupo
