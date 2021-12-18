// verification-helper: PROBLEM https://judge.yosupo.jp/problem/inv_of_formal_power_series
#include <cstdio>
#include <vector>
#include <array>

#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/simd/fps.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);
using mint = yosupo::modint998244353;

int main() {
    int n;
    sc.read(n);
    std::vector<mint> a(n);
    for (int i = 0; i < n; i++) {
        uint x;
        sc.read(x);
        a[i] = mint::raw(x);
    }
    yosupo::FPS<998244353> fa(a);

    std::vector<mint> c = fa.inv(n).to_vec();

    for (int i = 0; i < n; i++) {
        pr.write(c[i].val());
        pr.write(' ');
    }
    pr.writeln();
}
