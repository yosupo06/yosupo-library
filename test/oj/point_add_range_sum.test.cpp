// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/point_add_range_sum
// clang-format on

#include <cassert>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/segtree.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/types.hpp"

using namespace yosupo;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);

    int n, q;
    sc.read(n, q);

    std::vector<u64> a(n);
    for (int i = 0; i < n; i++) {
        sc.read(a[i]);
    }

    SegTree seg(a, Sum<u64>(0));

    for (int ph = 0; ph < q; ph++) {
        int t;
        sc.read(t);

        if (t == 0) {
            int p, x;
            sc.read(p, x);
            seg.set(p, seg.get(p) + x);
        } else {
            int l, r;
            sc.read(l, r);
            pr.writeln(seg.prod(l, r));
        }
    }
    return 0;
}