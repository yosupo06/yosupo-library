// verification-helper: PROBLEM https://judge.yosupo.jp/problem/matrix_product
#include <cstdio>

#include "atcoder/modint.hpp"
#include "yosupo/container/vector2d.hpp"
#include "yosupo/fastio.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);
using mint = atcoder::modint998244353;

int main() {
    int n, m, k;
    sc.read(n, m, k);

    yosupo::Vector2D<mint> a(n, m), b(m, k);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int x;
            sc.read(x);
            a[{i, j}] = x;
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            int x;
            sc.read(x);
            b[{i, j}] = x;
        }
    }

    auto c = a * b;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            if (j) pr.write(' ');
            pr.write(c[{i, j}].val());
        }
        pr.writeln();
    }
}
