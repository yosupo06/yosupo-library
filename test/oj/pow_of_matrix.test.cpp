// verification-helper: PROBLEM https://judge.yosupo.jp/problem/pow_of_matrix
#include <cstdio>

#include "atcoder/modint.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/vector2d.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);
using mint = atcoder::modint998244353;

int main() {
    int n;
    long long k;
    sc.read(n, k);

    yosupo::Vector2D<mint> a(n, n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            sc.read(x);
            a[{i, j}] = x;
        }
    }

    auto b = a.pow(k);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j) pr.write(' ');
            pr.write(b[{i, j}].val());
        }
        pr.writeln();
    }
}
