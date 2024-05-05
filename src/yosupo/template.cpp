#define YOSUPO_AVX2_PRAGMA
//#undef YOSUPO_LOCAL

#include "yosupo/fastio.hpp"
using namespace yosupo;

#include "yosupo/template.hpp"

Scanner sc = Scanner(stdin);
Printer pr = Printer(stdout);

struct RangeAddMax {
    using S = int;
    using F = int;
    S op(S a, S b) { return std::max(a, b); }
    S e() { return 0; }
    S rev(S a) { return a; }

    S mapping(F l, S r) { return l + r; }
    F composition(F l, F r) { return l + r; }
    F id() { return 0; }
};

int main() {
    int n;
    sc.read(n);
    V<ll> a(n);
    for (int i : iota(0, n)) {
        sc.read(a[i]);
    }
    ll ans = 0;
    for (int i : iota(0, n)) {
        ans -= (n - 1 - i) * a[i];
    }
    sort(a);
    for (int i : iota(0, n)) {
        ans += i * a[i];
    }
    pr.writeln(ans);
    return 0;
}
