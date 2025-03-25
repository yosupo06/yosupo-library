#pragma once

#include <initializer_list>
#include <limits>

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

}  // namespace yosupo