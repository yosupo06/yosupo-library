// verification-helper: PROBLEM https://judge.yosupo.jp/problem/convolution_mod
#include "yosupo/convolution.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);
using mint = yosupo::ModInt998244353;

int main() {
    int n, m;
    sc.read(n, m);
    std::vector<mint> a(n), b(m);
    for (int i = 0; i < n; i++) {
        int x;
        sc.read(x);
        a[i] = x;
    }
    for (int i = 0; i < m; i++) {
        int x;
        sc.read(x);
        b[i] = x;
    }
    auto c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    c = yosupo::convolution(a, b);
    for (int i = 0; i < n + m - 1; i++) {
        pr.write(c[i].val());
        if (i + 1 < n + m - 1) pr.write(' ');
    }
    pr.writeln();
}
