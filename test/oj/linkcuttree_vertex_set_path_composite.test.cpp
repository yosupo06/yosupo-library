// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/dynamic_tree_vertex_set_path_composite
// clang-format on

#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/linkcuttree.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"

using namespace yosupo;
using mint = ModInt998244353;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);
    int n, q;
    sc.read(n, q);
    using S = std::pair<mint, mint>;
    auto monoid = Monoid(S(1, 0), [](const S& l, const S& r) {
        return S(r.first * l.first, r.first * l.second + r.second);
    });
    std::vector<S> a(n);
    for (auto& f : a) {
        int b, c;
        sc.read(b, c);
        f = {b, c};
    }
    LinkCutTree tree(a, ActedMonoid(monoid));
    for (int i = 0; i < n - 1; i++) {
        int u, v;
        sc.read(u, v);
        tree.link(u, v);
    }
    for (int i = 0; i < q; i++) {
        int t;
        sc.read(t);
        if (t == 0) {
            int u, v, w, x;
            sc.read(u, v, w, x);
            tree.cut(u, v);
            tree.link(w, x);
        } else if (t == 1) {
            int p, b, c;
            sc.read(p, b, c);
            tree.set(p, {b, c});
        } else {
            int u, v, x;
            sc.read(u, v, x);
            auto [b, c] = tree.prod(u, v);
            pr.writeln((b * x + c).val());
        }
    }
}
