// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/associative_array
// clang-format on

#include <cstdio>

#include "yosupo/container/hashmap.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/types.hpp"

using namespace yosupo;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);

    IncrementalHashMap<u64, u64> a;

    int q;
    sc.read(q);
    for (int i = 0; i < q; i++) {
        int t;
        sc.read(t);

        if (t == 0) {
            u64 k, v;
            sc.read(k, v);

            a[k] = v;
        } else {
            u64 k;
            sc.read(k);

            auto it = a.find(k);
            if (it == a.end()) {
                pr.writeln(0);
            } else {
                pr.writeln(it->second);
            }
        }
    }
    return 0;
}
