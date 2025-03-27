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

            {
                auto tr1 = treem.make_leaf({x, 1});
                auto tr2 = treem.split(tree1, i);

                tree1 =
                    treem.merge(std::move(tree1),
                                treem.merge(std::move(tr1), std::move(tr2)));
            }
            {
                auto tr1 = treem.make_leaf({x, 1});
                auto tr2 = treem.split(tree2, size - i);

                tree2 =
                    treem.merge(std::move(tree2),
                                treem.merge(std::move(tr1), std::move(tr2)));
            }
        } else if (ty == 1) {
            int i;
            sc.read(i);

            {
                auto tr2 = treem.split(tree1, i);
                auto tr3 = treem.split(tr2, 1);

                tree1 = treem.merge(std::move(tree1), std::move(tr3));
            }
            {
                auto tr2 = treem.split(tree2, size - 1 - i);
                auto tr3 = treem.split(tr2, 1);

                tree2 = treem.merge(std::move(tree2), std::move(tr3));
            }
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

            {
                auto tr1 = treem.split(tree1, l);
                auto tr2 = treem.split(tr1, r - l);

                treem.all_apply(tr1, {b, c});

                tree1 =
                    treem.merge(std::move(tree1),
                                treem.merge(std::move(tr1), std::move(tr2)));
            }
            {
                auto tr1 = treem.split(tree2, size - r);
                auto tr2 = treem.split(tr1, r - l);

                treem.all_apply(tr1, {b, c});

                tree2 =
                    treem.merge(std::move(tree2),
                                treem.merge(std::move(tr1), std::move(tr2)));
            }
        } else if (ty == 4) {
            int l, r;
            sc.read(l, r);

            auto tr1 = treem.split(tree1, l);
            auto tr2 = treem.split(tr1, r - l);

            pr.writeln(treem.all_prod(tr1).first.val());

            tree1 = treem.merge(std::move(tree1),
                                treem.merge(std::move(tr1), std::move(tr2)));
        }
    }
    return 0;
}
