// verification-helper: PROBLEM https://judge.yosupo.jp/problem/lca

#include "yosupo/fastio.hpp"
#include "yosupo/hl.hpp"

yosupo::Scanner sc = yosupo::Scanner(stdin);
yosupo::Printer pr = yosupo::Printer(stdout);

int main() {
    int n, q;
    sc.read(n, q);

    yosupo::HLEulerTour hl(n);
    for (int i = 1; i < n; i++) {
        int p;
        sc.read(p);
        hl.add_edge(p, i);
    }
    hl.build();

    for (int i = 0; i < q; i++) {
        int u, v;
        sc.read(u, v);
        int w0 = hl.lca(u, v);
//        int w1 = hl.lca(v, u);
//        assert(w0 == w1);
        pr.writeln(w0);
    }
    return 0;
}
