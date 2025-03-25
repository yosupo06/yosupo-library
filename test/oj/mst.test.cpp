// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/minimum_spanning_tree
// clang-format on

#include "yosupo/mst.hpp"

#include "yosupo/fastio.hpp"

using ll = long long;

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int n, m;
    sc.read(n, m);

    yosupo::MST<ll> g(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        ll c;
        sc.read(a, b, c);
        g.add_edge(a, b, c);
    }

    auto result = g.mst();

    pr.writeln(result.first);
    for (auto i : result.second) {
        pr.write(i, ' ');
    }
    pr.writeln();
}
