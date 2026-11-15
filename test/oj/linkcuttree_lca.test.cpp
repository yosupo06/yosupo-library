// verification-helper: PROBLEM https://judge.yosupo.jp/problem/lca

#include "yosupo/algebra.hpp"
#include "yosupo/container/linkcuttree.hpp"
#include "yosupo/fastio.hpp"

using namespace yosupo;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);
    int n, q;
    sc.read(n, q);
    LinkCutTree tree(n, ActedMonoid(NoOpMonoid()));
    for (int i = 1; i < n; i++) {
        int p;
        sc.read(p);
        tree.link(i, p);
    }
    for (int i = 0; i < q; i++) {
        int u, v;
        sc.read(u, v);
        pr.writeln(tree.lca(u, v));
    }
}
