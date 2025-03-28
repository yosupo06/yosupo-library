// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/range_affine_range_sum
// clang-format on

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/dynamicsegtree.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"

using namespace std;
using namespace yosupo;

Scanner sc(stdin);
Printer pr(stdout);
using mint = ModInt998244353;

int main() {
    using S = pair<mint, mint>;
    auto monoid = Monoid(S(0, 0), [](S l, S r) {
        return S(l.first + r.first, l.second + r.second);
    });
    using F = pair<mint, mint>;
    auto act = Monoid(F(1, 0), [](F l, F r) {
        return F(l.first * r.first, l.first * r.second + l.second);
    });
    auto mapping = [](F l, S r) {
        return S(l.first * r.first + l.second * r.second, r.second);
    };
    DynamicSegtree treem(ActedMonoid(monoid, act, mapping));

    int n, q;
    sc.read(n, q);

    auto tree = treem.empty(n);
    for (int i = 0; i < n; i++) {
        int a;
        sc.read(a);
        treem.set(tree, i, {a, 1});
    }

    for (int ph = 0; ph < q; ph++) {
        int ty;
        sc.read(ty);

        if (ty == 0) {
            int l, r, b, c;
            sc.read(l, r, b, c);

            treem.apply(tree, l, r, {b, c});
        } else if (ty == 1) {
            int l, r;
            sc.read(l, r);

            auto ans = treem.prod(tree, l, r);
            pr.writeln(ans.first.val());
        } else {
            assert(false);
        }
    }
    return 0;
}
