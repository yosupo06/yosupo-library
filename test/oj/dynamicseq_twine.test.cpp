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
    auto act = Monoid(S(1, 0), [](S l, S r) {
        return S(l.first * r.first, l.first * r.second + l.second);
    });
    auto mapping = [](S l, S r) {
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

            auto tr1 = treem.split(tree1, l);
            auto tr2 = treem.split(tr1, r - l);

            auto tr3 = treem.split(tree2, size - r);
            auto tr4 = treem.split(tr3, r - l);

            tree1 = treem.merge(std::move(tree1),
                                treem.merge(std::move(tr3), std::move(tr2)));
            tree2 = treem.merge(std::move(tree2),
                                treem.merge(std::move(tr1), std::move(tr4)));
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
