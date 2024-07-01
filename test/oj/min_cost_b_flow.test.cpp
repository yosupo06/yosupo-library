// verification-helper: PROBLEM https://judge.yosupo.jp/problem/min_cost_b_flow
#include "yosupo/fastio.hpp"
#include "yosupo/networksimplex.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

using ll = long long;
int main() {
    int n, m;
    sc.read(n, m);
    yosupo::NetworkSimplexGraph<ll, ll> g(n);
    for (int i : std::views::iota(0, n)) {
        ll b;
        sc.read(b);
        g.add_excess(i, b);
    }
    for (int i = 0; i < m; i++) {
        int s, t;
        ll l, u, c;
        sc.read(s, t, l, u, c);
        g.add_edge(s, t, l, u, c);
    }
    auto r = solve<__int128>(std::move(g));

    if (!r.feasible) {
        pr.writeln("infeasible");
        return 0;
    }
    pr.writeln(r.cost);
    for (int i : std::views::iota(0, n)) {
        pr.writeln(r.potential[i]);
    }
    for (int i : std::views::iota(0, m)) {
        pr.writeln(r.flow[i]);
    }
}
