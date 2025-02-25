// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/sort_points_by_argument
// clang-format on

#include <algorithm>
#include <vector>

#include "yosupo/fastio.hpp"
#include "yosupo/geo/point.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

using P = yosupo::Point<long long>;

int main() {
    int n;
    sc.read(n);

    std::vector<P> ps(n);
    for (int i = 0; i < n; i++) {
        long long x, y;
        sc.read(x, y);
        ps[i] = P(x, y);
    }

    std::ranges::sort(ps, [&](const auto& a, const auto& b) {
        return yosupo::cmp_arg(a, b) < 0;
    });

    for (const auto& p : ps) {
        pr.writeln(p.x, ' ', p.y);
    }
}
