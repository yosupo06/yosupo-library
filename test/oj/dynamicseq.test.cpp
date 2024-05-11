// verification-helper: PROBLEM
// https://judge.yosupo.jp/problem/dynamic_sequence_range_affine_range_sum

#include <utility>

#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/splaytree.hpp"

using namespace std;
using namespace yosupo;

Scanner sc(stdin);
Printer pr(stdout);
using mint = modint998244353;

struct RangeAffineRangeSum {
    using S = pair<mint, mint>;
    using F = pair<mint, mint>;
    S op(S a, S b) { return {a.first + b.first, a.second + b.second}; }
    S e() { return {0, 0}; }
    S rev(S a) { return a; }

    S mapping(F l, S r) {
        return {l.first * r.first + l.second * r.second, r.second};
    }
    F composition(F l, F r) {
        return {l.first * r.first, l.first * r.second + l.second};
    }
    F id() { return {1, 0}; }
};

int main() {
    SplayTree treem((RangeAffineRangeSum{}));

    int n, q;
    sc.read(n, q);

    std::vector<RangeAffineRangeSum::S> v(n);
    for (int i = 0; i < n; i++) {
        int a;
        sc.read(a);
        v[i] = {a, 1};
    }

    auto tr = treem.build(v);

    for (int ph = 0; ph < q; ph++) {
        int ty;
        sc.read(ty);

        if (ty == 0) {
            int i, x;
            sc.read(i, x);

            auto tr2 = treem.make_leaf({x, 1});
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

            auto tr2 = treem.split(tr, l);
            auto tr3 = treem.split(tr2, r - l);

            treem.reverse(tr2);

            tr2 = treem.merge(std::move(tr2), std::move(tr3));
            tr = treem.merge(std::move(tr), std::move(tr2));
        } else if (ty == 3) {
            int l, r, b, c;
            sc.read(l, r, b, c);

            auto tr2 = treem.split(tr, l);
            auto tr3 = treem.split(tr2, r - l);

            treem.all_apply(tr2, {b, c});

            tr2 = treem.merge(std::move(tr2), std::move(tr3));
            tr = treem.merge(std::move(tr), std::move(tr2));
        } else if (ty == 4) {
            int l, r;
            sc.read(l, r);

            auto tr2 = treem.split(tr, l);
            auto tr3 = treem.split(tr2, r - l);

            pr.writeln(treem.all_prod(tr2).first.val());

            tr2 = treem.merge(std::move(tr2), std::move(tr3));
            tr = treem.merge(std::move(tr), std::move(tr2));
        }
    }
    return 0;
}
