#pragma once

#include <bit>
#include <concepts>

namespace yosupo {
using std::countr_zero;

inline int countr_zero(unsigned __int128 x) {
    auto lo = (unsigned long long)(x);
    auto hi = (unsigned long long)(x >> 64);
    return lo ? std::countr_zero(lo) : 64 + std::countr_zero(hi);
}

template <class T>
    requires requires(T x) {
        { x.countr_zero() } -> std::same_as<int>;
    }
int countr_zero(T x) {
    return x.countr_zero();
}

}  // namespace yosupo
