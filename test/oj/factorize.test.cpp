// verification-helper: PROBLEM https://judge.yosupo.jp/problem/factorize
#include <algorithm>
#include <cstdio>

#include "yosupo/fastio.hpp"
#include "yosupo/math/factor.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int q;
    sc.read(q);

    for (int i = 0; i < q; i++) {
        long long a;
        sc.read(a);
        auto v = yosupo::factor(a);
        sort(v.begin(), v.end());
        pr.write(v.size());
        for (auto x : v) {
            pr.write(' ');
            pr.write(x);
        }
        pr.writeln();
    }
}
