#pragma once

#include <cassert>
#include <utility>
#include <vector>

namespace yosupo {

struct RollbackDSU {
    explicit RollbackDSU(int _n) : n(_n), parent_or_size(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        history.emplace_back(x, parent_or_size[x]);
        history.emplace_back(y, parent_or_size[y]);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        return leader(a) == leader(b);
    }

    int leader(int a) {
        assert(0 <= a && a < n);
        if (parent_or_size[a] < 0) return a;
        return leader(parent_or_size[a]);
    }

    int size(int a) {
        assert(0 <= a && a < n);
        return -parent_or_size[leader(a)];
    }

    template <class F> void with(F f) {
        int checkpoint = int(history.size());
        f();
        while (int(history.size()) > checkpoint) {
            auto [idx, val] = history.back();
            history.pop_back();
            parent_or_size[idx] = val;
        }
    }

  private:
    int n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<int> parent_or_size;
    std::vector<std::pair<int, int>> history;
};

}  // namespace yosupo
