#include <sys/types.h>
#include <bit>
#include <cassert>
#include <vector>

#include "yosupo/algebra.hpp"

namespace yosupo {

template <monoid M> struct SparseTable {
    using S = M::S;

    M m;
    std::vector<std::vector<S>> data;
    SparseTable(std::vector<S> d, M _m = M()) : m(_m) {
        int n = int(d.size());
        if (n == 0) return;
        int lg = 32 - std::countl_zero(uint(n));
        data = std::vector<std::vector<S>>(lg);
        data[0] = d;
        int l = 1;
        for (int s = 1; s < lg; s++) {
            data[s] = std::vector<S>(n);
            for (int i = 0; i < n - l; i++) {
                data[s][i] = m.op(data[s - 1][i], data[s - 1][i + l]);
            }
            l <<= 1;
        }
    }
    M::S query(int l, int r) {
        assert(l <= r);
        if (l == r) return m.e();
        int u = 31 - std::countl_zero(uint(r - l));
        return m.op(data[u][l], data[u][r - (1 << u)]);
    }
};

}  // namespace yosupo
