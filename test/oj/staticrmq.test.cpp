// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/staticrmq
// clang-format on

#include "yosupo/fastio.hpp"
#include "yosupo/sparsetable.hpp"

#include <ranges>
#include <vector>

using ll = long long;
using namespace std;

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int n, m;
    sc.read(n, m);

    std::vector<int> a(n);
    for (int i : views::iota(0, n)) {
        sc.read(a[i]);
    }
    yosupo::SparseTable<yosupo::Min<int>> sp(a);
    for (int i = 0; i < m; i++) {
        int l, r;
        sc.read(l, r);
        pr.writeln(sp.query(l, r));
    }
}
