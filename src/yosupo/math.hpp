#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <initializer_list>
#include <limits>
#include <numeric>
#include <vector>

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

template <class T> T floor_div(T x, T y) {
    auto d = x / y;
    auto r = x % y;
    if (r == 0) return d;
    if ((r > 0) == (y > 0)) return d;
    return d - 1;
}
template <class T> T ceil_div(T x, T y) {
    auto d = x / y;
    auto r = x % y;
    if (r == 0) return d;
    if ((r > 0) == (y > 0)) return d + 1;
    return d;
}

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

// integer k-th root: floor(a^(1/k))
u64 iroot(u64 a, int k) {
    if (k == 1) return a;

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
    u64 lw = 0;
    u64 up = 1LL << 33;
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

inline constexpr u32 smallest_primitive_root(u32 m) {
    if (m == 2) return 1;

    u32 divs[20] = {};
    int cnt = 0;
    u32 x = (m - 1) / 2;
    for (int i = 2; (u64)(i)*i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) {
                x /= i;
            }
        }
    }
    if (x > 1) {
        divs[cnt++] = x;
    }
    for (u32 g = 2;; g++) {
        bool ok = true;
        for (int i = 0; i < cnt; i++) {
            if (pow_mod_u64(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}

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
