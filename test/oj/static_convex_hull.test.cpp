// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/static_convex_hull
// clang-format on

#include <cstdio>
#include <vector>

#include "yosupo/fastio.hpp"
#include "yosupo/geo/convex.hpp"
#include "yosupo/geo/point.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

using P = yosupo::Point<long long>;

void solve() {
    int n;
    sc.read(n);

    std::vector<P> ps(n);
    for (int i = 0; i < n; i++) {
        long long x, y;
        sc.read(x, y);
        ps[i] = P(x, y);
    }

    auto hull = yosupo::convex(ps);

    pr.writeln(hull.size());
    for (const auto& p : hull) {
        pr.writeln(p.x, ' ', p.y);
    }
}

int main() {
    int t;
    sc.read(t);

    for (int i = 0; i < t; i++) {
        solve();
    }
}
