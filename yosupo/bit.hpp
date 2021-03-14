#pragma once

namespace yosupo {

namespace internal {

// @param n `0 <= n`
// @return minimum non-negative `x` s.t. `n <= 2**x`
int ceil_pow2(int n) {
    int x = 0;
    while ((1U << x) < (unsigned int)(n)) x++;
    return x;
}

}  // namespace internal

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned int n) { return __builtin_ctz(n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned long n) { return __builtin_ctzl(n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned long long n) { return __builtin_ctzll(n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned char n) { return bsf((unsigned int)n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned short n) { return bsf((unsigned int)n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned __int128 n) {
    unsigned long long low = (unsigned long long)(n);
    unsigned long long high = (unsigned long long)(n >> 64);
    return low ? __builtin_ctzll(low) : 64 + __builtin_ctzll(high);
}

// @param n `1 <= n`
// @return maximum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned int n) {
    return 8 * (int)sizeof(unsigned int) - 1 - __builtin_clz(n);
}
// @param n `1 <= n`
// @return maximum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned long n) {
    return 8 * (int)sizeof(unsigned long) - 1 - __builtin_clzl(n);
}
// @param n `1 <= n`
// @return maximum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned long long n) {
    return 8 * (int)sizeof(unsigned long long) - 1 - __builtin_clzll(n);
}
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned char n) { return bsr((unsigned int)n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned short n) { return bsr((unsigned int)n); }
// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsr(unsigned __int128 n) {
    unsigned long long low = (unsigned long long)(n);
    unsigned long long high = (unsigned long long)(n >> 64);
    return high ? 127 - __builtin_clzll(high) : 63 - __builtin_ctzll(low);
}

int popcnt(unsigned int n) { return __builtin_popcount(n); }
int popcnt(unsigned long n) { return __builtin_popcountll(n); }
int popcnt(unsigned char n) { return popcnt((unsigned int)n); }
int popcnt(unsigned short n) { return popcnt((unsigned int)n); }

}  // namespace yosupo
