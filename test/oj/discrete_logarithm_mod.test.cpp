// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/discrete_logarithm_mod
// clang-format on

#include <cstdio>

#include "yosupo/fastio.hpp"
#include "yosupo/math/logarithm.hpp"
#include "yosupo/types.hpp"

using namespace yosupo;

Scanner sc(stdin);
Printer pr(stdout);

int main() {
    int t;
    sc.read(t);
    for (int i = 0; i < t; i++) {
        u64 x, y, m;
        sc.read(x, y, m);
        u64 ans = yosupo::discrete_logarithm(x, y, m);
        pr.writeln((i64)ans);
    }
    return 0;
}
