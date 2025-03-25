#pragma once

#include <cmath>

#include "yosupo/types.hpp"

namespace yosupo {
// x * inv_u32(x) = 1 (mod 2^32)
inline constexpr u32 inv_u32(const u32 x) {
    u32 inv = 1;
    for (int i = 0; i < 5; i++) {
        inv *= 2u - inv * x;
    }
    return inv;
}

inline constexpr u64 pow_mod_u64(u64 x, u64 n, u64 m) {
    if (m == 1) return 0;
    u64 r = 1;
    u64 y = x % m;
    while (n) {
        if (n & 1) r = (u64)((u128)(1) * r * y % m);
        y = (u64)((u128)(1) * y * y % m);
        n >>= 1;
    }
    return r;
}

inline u64 isqrt(u64 a) {
    if (a == u64(-1)) return (u64(1) << 32) - 1;
    u64 x = (u64)std::sqrt((double)a);
    return x * x > a ? x - 1 : x;
}

// integer k-th root: floor(a^(1/k))
inline u64 iroot(u64 a, int k) {
    if (k == 2) return isqrt(a);
    if (k == 1 || a == 0) return a;
    if (k >= 64 || a < (u64(1) << k)) return 1;

    // is (b^k <= a)?
    auto check = [&](u64 b) {
        u128 result = 1;
        for (int i = 0; i < k; i++) {
            result *= b;
            if (result > a) return false;
        }
        return true;
    };

    // lw^k <= a < up^k
    u64 lw = 2;
    u64 up = 1LL << ((64 + k - 1) / k);
    while (up - lw > 1) {
        u64 mid = (lw + up) / 2;
        if (check(mid)) {
            lw = mid;
        } else {
            up = mid;
        }
    }

    return lw;
}

}  // namespace yosupo