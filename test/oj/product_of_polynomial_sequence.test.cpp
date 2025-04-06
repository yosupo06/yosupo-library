// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/product_of_polynomial_sequence
// clang-format on

#include <vector>

#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/modvec.hpp"

using namespace yosupo;

using mint = ModInt998244353;
using mvec = ModVec998244353;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);

    int n;
    sc.read(n);

    std::vector<mvec> a(n);

    for (int i = 0; i < n; i++) {
        int m;
        sc.read(m);
        mvec b(m + 1);
        for (int j = 0; j <= m; j++) {
            int x;
            sc.read(x);
            b[j] = x;
        }
        a[i] = b;
    }

    auto c = mvec::prod(a);

    for (int i = 0; i < int(c.size()); i++) {
        if (i) pr.write(' ');
        pr.write(c[i].val());
    }
    pr.writeln();
}
