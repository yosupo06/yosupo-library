#pragma once

#include <algorithm>
#include <initializer_list>

#include "yosupo/bit.hpp"
namespace yosupo {

// binary gcd
unsigned long long gcd(unsigned long long a, unsigned long long b) {
    if (a == 0) return b;
    if (b == 0) return a;
    int shift = bsf(a | b);
    a >>= bsf(a);
    do {
        b >>= bsf(b);
        if (a > b) std::swap(a, b);
        b -= a;
    } while (b);
    return (a << shift);
}
long long gcd(long long a, long long b) {
    return gcd(abs(a), abs(b));
}

unsigned long long pow_mod_u64(unsigned long long x, unsigned long long n, unsigned long long m) {
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

bool is_prime(unsigned int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
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

bool is_prime(unsigned long long n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    unsigned long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    for (unsigned long long a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
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

long long pollard_single(long long n) {
    auto f = [&](ll x) { return (__int128_t(x) * x + 1) % n; };
    if (is_prime(n)) return n;
    if (n % 2 == 0) return 2;
    ll st = 0;
    while (true) {
        st++;
        ll x = st, y = f(x);
        while (true) {
            ll p = gcd((y - x + n), n);
            if (p == 0 || p == n) break;
            if (p != 1) return p;
            x = f(x);
            y = f(f(y));
        }
    }
}

std::vector<long long> factor(long long n) {
    if (n == 1) return {};
    ll x = pollard_single(n);
    if (x == n) return {x};
    auto f0 = factor(x), f1 = factor(n / x);
    f0.insert(f0.end(), f1.begin(), f1.end());
    return f0;
}

}
