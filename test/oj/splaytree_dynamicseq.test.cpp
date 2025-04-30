// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/dynamic_sequence_range_affine_range_sum
// clang-format on

#include <algorithm>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/splaytree.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"

using namespace yosupo;

using mint = ModInt998244353;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);

    using S = std::pair<mint, mint>;
    auto monoid = Monoid(S(0, 0), [](const S& l, const S& r) {
        return S(l.first + r.first, l.second + r.second);
    });
    using F = std::pair<mint, mint>;
    auto act = Monoid(F(1, 0), [](const F& l, const F& r) {
        return F(l.first * r.first, l.first * r.second + l.second);
    });
    auto mapping = [](const F& l, const S& r) {
        return S(l.first * r.first + l.second * r.second, r.second);
    };
    SplayTree treem(ActedMonoid(monoid, act, mapping));

    int n, q;
    sc.read(n, q);

    std::vector<S> v(n);
    for (int i = 0; i < n; i++) {
        int a;
        sc.read(a);
        v[i] = {a, 1};
    }

    auto tree1 = treem.build(v);
    std::ranges::reverse(v);
    auto tree2 = treem.build(v);

    for (int ph = 0; ph < q; ph++) {
        int size = int(treem.size(tree1));
        int ty;
        sc.read(ty);

        if (ty == 0) {
            int i, x;
            sc.read(i, x);

            treem.insert(tree1, i, {x, 1});
            treem.insert(tree2, size - i, {x, 1});
        } else if (ty == 1) {
            int i;
            sc.read(i);

            treem.erase(tree1, i);
            treem.erase(tree2, size - 1 - i);
        } else if (ty == 2) {
            int l, r;
            sc.read(l, r);

            auto [tr1, tr2, tr3] = treem.split3(std::move(tree1), l, r);
            auto [tr4, tr5, tr6] =
                treem.split3(std::move(tree2), size - r, size - l);

            tree1 =
                treem.merge3(std::move(tr1), std::move(tr5), std::move(tr3));
            tree2 =
                treem.merge3(std::move(tr4), std::move(tr2), std::move(tr6));
        } else if (ty == 3) {
            int l, r, b, c;
            sc.read(l, r, b, c);

            treem.apply(tree1, l, r, {b, c});
            treem.apply(tree2, size - r, size - l, {b, c});
        } else if (ty == 4) {
            int l, r;
            sc.read(l, r);

            pr.writeln(treem.prod(tree1, l, r).first.val());
        }
    }
    return 0;
}
