// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/dynamic_sequence_range_affine_range_sum
// clang-format on

#include <concepts>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/reversiblesplaytree.hpp"
#include "yosupo/fastio.hpp"
// #include "yosupo/modint.hpp"
#include "atcoder/modint.hpp"

using namespace std;
using namespace yosupo;

Scanner sc(stdin);
Printer pr(stdout);
// using mint = ModInt998244353;
using mint = atcoder::modint998244353;

template <monoid Monoid, monoid Act, class Mapping>
    requires requires(Mapping mapping,
                      const typename Act::S& a,
                      const typename Monoid::S& b,
                      int c) {
        { mapping(a, b, c) } -> std::same_as<typename Monoid::S>;
    }
struct ActedMonoid2 {
    using S = Monoid::S;
    using F = Act::S;

    explicit ActedMonoid2(const Monoid& _monoid = Monoid(),
                          const Act& _act = Act(),
                          const Mapping& _mapping = Mapping())
        : monoid(_monoid), act(_act), mapping(_mapping) {}

    Monoid monoid;
    Act act;
    Mapping mapping;
};

int main() {
    using S = mint;
    auto monoid = Monoid(S(0), [](const S& l, const S& r) { return S(l + r); });
    using F = std::pair<mint, mint>;
    auto act = Monoid(F(1, 0), [](const F& l, const F& r) {
        return F(l.first * r.first, l.first * r.second + l.second);
    });
    auto mapping = [](const F& l, const S& r, int len) {
        return S(l.first * r + l.second * mint::raw(len));
    };

    SplayTree treem(ActedMonoid2(monoid, act, mapping));

    int n, q;
    sc.read(n, q);

    std::vector<S> v(n);
    for (int i = 0; i < n; i++) {
        int a;
        sc.read(a);
        v[i] = a;
    }

    auto tr = treem.build(v);

    for (int ph = 0; ph < q; ph++) {
        int ty;
        sc.read(ty);

        if (ty == 0) {
            int i, x;
            sc.read(i, x);

            auto tr2 = treem.build(x);
            auto tr3 = treem.split(tr, i);

            tr2 = treem.merge(std::move(tr2), std::move(tr3));
            tr = treem.merge(std::move(tr), std::move(tr2));
        } else if (ty == 1) {
            int i;
            sc.read(i);

            auto tr2 = treem.split(tr, i);
            auto tr3 = treem.split(tr2, 1);

            tr = treem.merge(std::move(tr), std::move(tr3));
        } else if (ty == 2) {
            int l, r;
            sc.read(l, r);
            auto [tr1, tr2, tr3] = treem.split3(std::move(tr), l, r);
            treem.reverse(tr2);
            tr = treem.merge3(std::move(tr1), std::move(tr2), std::move(tr3));
        } else if (ty == 3) {
            int l, r, b, c;
            sc.read(l, r, b, c);

            treem.apply(tr, l, r, {b, c});
        } else if (ty == 4) {
            int l, r;
            sc.read(l, r);
            pr.writeln(treem.prod(tr, l, r).val());
        }
    }
    return 0;
}
