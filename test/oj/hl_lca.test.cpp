// verification-helper: PROBLEM https://judge.yosupo.jp/problem/lca

#include <utility>
#include "yosupo/fastio.hpp"
#include "yosupo/hl.hpp"
#include "yosupo/tree.hpp"

yosupo::Scanner sc = yosupo::Scanner(stdin);
yosupo::Printer pr = yosupo::Printer(stdout);

int main() {
    int n, q;
    sc.read(n, q);

    yosupo::RootedTreeBuilder tree(n);
    for (int i = 1; i < n; i++) {
        int p;
        sc.read(p);
        tree.add_edge(p, i);
    }
    yosupo::HLEulerTour hl(std::move(tree).build(0));

    for (int i = 0; i < q; i++) {
        int u, v;
        sc.read(u, v);
        int w = hl.lca(u, v);
        pr.writeln(w);
    }
    return 0;
}
