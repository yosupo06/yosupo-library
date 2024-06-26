#pragma once

#include <algorithm>
#include <bit>
#include <initializer_list>
#include <limits>
#include <vector>

namespace yosupo {

// binary gcd
inline unsigned long long gcd(unsigned long long a, unsigned long long b) {
    if (a == 0) return b;
    if (b == 0) return a;
    int shift;
    {
        int a_bsf = std::countr_zero(a);
        a >>= a_bsf;
        int b_bsf = std::countr_zero(b);
        b >>= b_bsf;
        shift = std::min(a_bsf, b_bsf);
    }
    while (a != b) {
        if (a > b) std::swap(a, b);
        b -= a;
        b >>= std::countr_zero(b);
    }
    return (a << shift);
}
inline long long gcd(long long a, long long b) {
    unsigned long long _a = a, _b = b;
    if ((unsigned long long)std::numeric_limits<long long>::max() < _a)
        _a = -_a;
    if ((unsigned long long)std::numeric_limits<long long>::max() < _b)
        _b = -_b;
    return gcd(_a, _b);
}
inline unsigned int gcd(unsigned int a, unsigned int b) {
    return (unsigned int)gcd((unsigned long long)a, (unsigned long long)b);
}
inline int gcd(int a, int b) { return (int)gcd((long long)a, (long long)b); }

// @param m `1 <= m`
// @return x ** n % m
inline unsigned long long pow_mod_u64(unsigned long long x,
                               unsigned long long n,
                               unsigned long long m) {
    if (m == 1) return 0;
    unsigned long long r = 1;
    unsigned long long y = x % m;
    while (n) {
        if (n & 1) r = (unsigned long long)((unsigned __int128)(1) * r * y % m);
        y = (unsigned long long)((unsigned __int128)(1) * y * y % m);
        n >>= 1;
    }
    return r;
}

inline bool is_prime(unsigned int n) {
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    unsigned long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    for (unsigned long long a : {2, 7, 61}) {
        if (a % n == 0) return true;
        unsigned long long t = d;
        unsigned long long y = pow_mod_u64(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = (unsigned long long)((unsigned __int128)(1) * y * y % n);
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
inline bool is_prime(unsigned long long n) {
    if (n <= std::numeric_limits<unsigned int>::max()) {
        return is_prime((unsigned int)n);
    }
    if (n % 2 == 0) return false;
    unsigned long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    for (unsigned long long a :
         {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        if (a % n == 0) return true;
        unsigned long long t = d;
        unsigned long long y = pow_mod_u64(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = (unsigned long long)((unsigned __int128)(1) * y * y % n);
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
inline bool is_prime(int n) {
    if (n <= 1) return false;
    return is_prime((unsigned int)n);
}
inline bool is_prime(long long n) {
    if (n <= 1) return false;
    return is_prime((unsigned long long)n);
}

inline long long pollard_single(long long n) {
    if (is_prime((unsigned long long)n)) return n;
    if (n % 2 == 0) return 2;
    long long st = 0;
    auto f = [&](long long x) {
        return (long long)(((unsigned __int128)(x)*x + st) % n);
    };
    while (true) {
        st++;
        long long x = st, y = f(x);
        while (true) {
            long long p = gcd((y - x + n), n);
            if (p == 0 || p == n) break;
            if (p != 1) return p;
            x = f(x);
            y = f(f(y));
        }
    }
}

inline std::vector<long long> factor(long long n) {
    if (n == 1) return {};
    long long x = pollard_single(n);
    if (x == n) return {x};
    auto f0 = factor(x), f1 = factor(n / x);
    f0.insert(f0.end(), f1.begin(), f1.end());
    return f0;
}

}  // namespace yosupo
