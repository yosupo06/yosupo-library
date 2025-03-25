#pragma once

#include <vector>

#include "yosupo/math/prime.hpp"
#include "yosupo/numeric.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

inline i64 pollard_single(i64 n) {
    if (is_prime(n)) return n;
    if (n % 2 == 0) return 2;
    long long st = 0;
    auto f = [&](i64 x) { return (i64)(((u128)(x)*x + st) % n); };
    while (true) {
        st++;
        i64 x = st, y = f(x);
        while (true) {
            i64 p = gcd((y - x + n), n);
            if (p == 0 || p == n) break;
            if (p != 1) return p;
            x = f(x);
            y = f(f(y));
        }
    }
}

inline std::vector<i64> factor(i64 n) {
    if (n == 1) return {};
    i64 x = pollard_single(n);
    if (x == n) return {x};
    auto f0 = factor(x), f1 = factor(n / x);
    f0.insert(f0.end(), f1.begin(), f1.end());
    return f0;
}

}  // namespace yosupo