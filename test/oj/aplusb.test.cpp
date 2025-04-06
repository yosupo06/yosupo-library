// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/many_aplusb
// clang-format on

#include <cstdio>

#include "yosupo/fastio.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int t;
    sc.read(t);

    for (int i = 0; i < t; i++) {
        long long a, b;
        sc.read(a, b);
        pr.writeln(a + b);
    }
}
