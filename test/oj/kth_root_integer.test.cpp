// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/kth_root_integer
// clang-format on

#include "yosupo/fastio.hpp"
#include "yosupo/math/basic.hpp"
#include "yosupo/types.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int t;
    sc.read(t);
    for (int i = 0; i < t; i++) {
        yosupo::u64 a;
        int k;
        sc.read(a, k);
        pr.writeln(yosupo::iroot(a, k));
    }
}
