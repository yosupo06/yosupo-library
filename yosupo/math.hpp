#pragma once

#include <initializer_list>

namespace yosupo {

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

}
