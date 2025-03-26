// verification-helper: PROBLEM https://judge.yosupo.jp/problem/matrix_rank

#include "atcoder/modint.hpp"
#include "yosupo/container/vector2d.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/matrix.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);
using mint = atcoder::modint998244353;

int main() {
    int n, m;
    sc.read(n, m);

    yosupo::Vector2D<mint> a(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int x;
            sc.read(x);
            a[{i, j}] = x;
        }
    }

    pr.writeln(yosupo::rank(a));
}
