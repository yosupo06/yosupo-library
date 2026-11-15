// clang-format off
// verification-helper: PROBLEM https://atcoder.jp/contests/abc127/tasks/abc127_f
// clang-format on
#include <cstdio>

#include "yosupo/fastio.hpp"
#include "yosupo/slopetrick.hpp"

yosupo::Scanner sc(stdin);
yosupo::Printer pr(stdout);

int main() {
    int q;
    sc.read(q);
    yosupo::SlopeTrick<long long> f;
    for (int i = 0; i < q; i++) {
        int type;
        sc.read(type);
        if (type == 1) {
            long long a, b;
            sc.read(a, b);
            f.add_abs(a);
            f.add_const(b);
        } else {
            pr.writeln(*f.argmin().lower, ' ', f.min());
        }
    }
}
