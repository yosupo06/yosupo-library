#pragma once

#include <initializer_list>
#include <limits>
#include <vector>

#include "yosupo/math/basic.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

inline bool is_prime(u32 n) {
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    u64 d = n - 1;
    while (d % 2 == 0) d /= 2;
    for (u64 a : {2, 7, 61}) {
        if (a % n == 0) return true;
        u64 t = d;
        u64 y = pow_mod_u64(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = (u64)((u128)(1) * y * y % n);
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
inline bool is_prime(u64 n) {
    if (n <= std::numeric_limits<u32>::max()) {
        return is_prime((u32)n);
    }
    if (n % 2 == 0) return false;
    u64 d = n - 1;
    while (d % 2 == 0) d /= 2;
    for (u64 a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        if (a % n == 0) return true;
        u64 t = d;
        u64 y = pow_mod_u64(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = (u64)((u128)(1) * y * y % n);
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
inline bool is_prime(i32 n) {
    if (n <= 1) return false;
    return is_prime((u32)n);
}
inline bool is_prime(i64 n) {
    if (n <= 1) return false;
    return is_prime((u64)n);
}

// lpf_table[i] = the smallest prime factor of i
inline std::vector<i32> lpf_table(i32 n) {
    std::vector<i32> table(n + 1, -1);
    std::vector<int> primes;
    for (int d = 2; d <= n; d++) {
        if (table[d] == -1) {
            primes.push_back(d);
            table[d] = d;
        }
        for (int p : primes) {
            if (p * d > n || p > table[d]) break;
            table[p * d] = p;
        }
    }
    return table;
}

// enumerate primes no more than n
inline std::vector<i32> primes(i32 n) {
    std::vector<i32> table = lpf_table(n);
    std::vector<i32> p;
    for (int i = 2; i <= n; i++) {
        if (table[i] == i) p.push_back(i);
    }
    return p;
}

}  // namespace yosupo