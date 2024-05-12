// verification-helper: PROBLEM https://judge.yosupo.jp/problem/general_matching

#include "yosupo/matching.hpp"
#include "yosupo/fastio.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int n, m;
    sc.read(n, m);

    yosupo::MaxMatching g(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        sc.read(a, b);
        g.add_edge(a, b);
    }

    auto result = g.max_matching();

    pr.writeln(result.size());
    for (auto p : result) {
        pr.writeln(p.first, ' ', p.second);
    }
}
