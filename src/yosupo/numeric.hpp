#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <numeric>
#include <type_traits>
#include <utility>

#include "yosupo/bit.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

// sign
template <class T>
    requires std::is_integral_v<T>
int sgn(T x) {
    if (x == 0) return 0;
    return x > 0 ? 1 : -1;
}
inline int sgn(__int128 x) {
    if (x == 0) return 0;
    return x > 0 ? 1 : -1;
}
// for custom class
template <class T>
    requires requires(T x) {
        { x.sgn() } -> std::same_as<int>;
    }
int sgn(T x) {
    return x.sgn();
}

// abs
template <std::integral T> inline T abs(T x) { return std::abs(x); }
inline i128 abs(i128 x) { return x < 0 ? -x : x; }
template <class T>
    requires requires(T x) {
        { x.abs() } -> std::same_as<T>;
    }
T abs(T x) {
    return x.abs();
}

// gcd
using std::gcd;
inline u128 gcd(u128 a, u128 b) {
    if (a == 0) return b;
    if (b == 0) return a;
    int shift;
    {
        int a_bsf = countr_zero(a);
        a >>= a_bsf;
        int b_bsf = countr_zero(b);
        b >>= b_bsf;
        shift = std::min(a_bsf, b_bsf);
    }
    while (a != b) {
        if (a > b) std::swap(a, b);
        b -= a;
        b >>= countr_zero(b);
    }
    return (a << shift);
}
inline i128 gcd(i128 a, i128 b) { return gcd((u128)abs(a), (u128)abs(b)); }
template <class T>
    requires requires(T x) {
        { T::gcd(x, x) } -> std::same_as<T>;
    }
T gcd(T x, T y) {
    return T::gcd(x, y);
}

}  // namespace yosupo