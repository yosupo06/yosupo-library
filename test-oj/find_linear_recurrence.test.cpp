// verification-helper: PROBLEM https://judge.yosupo.jp/problem/find_linear_recurrence
#include <cstdio>
#include <vector>

#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/fps.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);
using mint = yosupo::modint998244353;

int main() {
    int n;
    sc.read(n);

    std::vector<mint> a(n);
    for (int i = 0; i < n; i++) {
        int x;
        sc.read(x);
        a[i] = x;
    }

    auto c = yosupo::berlekamp_massey(a);

    int m = c.size();

    pr.writeln(m - 1);
    for (int i = 0; i < m - 1; i++) {
        pr.write(c.freq(m - 2 - i).val());
        pr.write(' ');
    }
    pr.writeln();
}
