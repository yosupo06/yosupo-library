#pragma once

#include "yosupo/container/hashmap.hpp"
#include "yosupo/math/basic.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

inline u64 discrete_logarithm(u64 start, u64 x, u64 y, u64 m) {
    const u64 STEP = 32000;

    auto mul = [&](u64 a, u64 b) { return (a * b) % m; };
    IncrementalHashMap<u64, u64> mp;
    u64 big = 1;  // x^m
    for (u64 i = 0; i < STEP; i++) {
        mp[y] = i;
        y = mul(y, x);
        big = mul(big, x);
    }
    for (u64 step = 0; step < m + 10; step += STEP) {
        start = mul(start, big);
        // check [step + 1, step + STEP]
        if (mp.count(start)) {
            return (step + STEP) - mp[start];
        }
    }
    return -1;
}

inline u64 discrete_logarithm(u64 x, u64 y, u64 m) {
    if (m == 1) return 0;
    auto mul = [&](i64 a, i64 b) { return (a * b) % m; };
    u64 start = 1;
    for (int i = 0; i < 100; i++) {
        if (start == y) return i;
        start = mul(start, x);
    }
    u64 pred = 100 + discrete_logarithm(start, x, y, m);
    if (pow_mod_u64(x, pred, m) != y) return -1;
    return pred;
}

}  // namespace yosupo
