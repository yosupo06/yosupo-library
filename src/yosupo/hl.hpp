#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/flattenvector.hpp"
#include "yosupo/tree.hpp"

namespace yosupo {

struct HLEulerTour {
  public:
    int n, root;
    std::vector<int> ord, rord;

    explicit HLEulerTour(const RootedTree& tree) : n(tree.n), root(tree.root) {
        auto max_ch = std::vector<int>(n, -1);
        std::vector<int> size(n, 1);
        for (int i = n - 1; i >= 1; i--) {
            int u = tree.topo[i];
            size[tree.par[u]] += size[u];
            if (max_ch[tree.par[u]] == -1 ||
                size[u] > size[max_ch[tree.par[u]]]) {
                max_ch[tree.par[u]] = u;
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

            if (tree.par[u] == -1 || max_ch[tree.par[u]] != u) {
                nxt[i] = (tree.par[u] == -1 ? -1 : ord[tree.par[u]]) * 2;
            } else {
                nxt[i] = (nxt[i - 1] & 1) ? nxt[i - 1] : (i - 1) * 2 + 1;
            }

            for (int v : tree.children.at(u)) {
                if (v == tree.par[u] || v == max_ch[u]) continue;
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
    // key / value are ordinal
    std::vector<int> nxt, _size;
};

}  // namespace yosupo
