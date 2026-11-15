// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/dynamic_tree_vertex_add_path_sum
// clang-format on

#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/linkcuttree.hpp"
#include "yosupo/fastio.hpp"

using namespace yosupo;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);
    int n, q;
    sc.read(n, q);
    std::vector<long long> a(n);
    for (auto& x : a) sc.read(x);
    LinkCutTree tree(a, ActedMonoid(Sum<long long>(0)));
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
            int p;
            long long x;
            sc.read(p, x);
            tree.set(p, tree.get(p) + x);
        } else {
            int u, v;
            sc.read(u, v);
            pr.writeln(tree.prod(u, v));
        }
    }
}
