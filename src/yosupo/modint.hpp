#pragma once

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "yosupo/math.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <i32 MOD> struct ModInt {
    static_assert(MOD % 2 && 1 <= MOD && MOD <= (1 << 30) - 1,
                  "mod must be odd and at most 2^30 - 1");

    static constexpr i32 mod() { return MOD; }

    constexpr ModInt() : x(0) {}

    constexpr explicit ModInt(u32 _x) : x(reduce_mul(_x, B2)) {}
    constexpr ModInt(std::signed_integral auto _x)
        : ModInt((u32)(_x % MOD + MOD)) {}
    constexpr ModInt(std::unsigned_integral auto _x)
        : ModInt((u32)(_x % MOD)) {}

    constexpr i32 val() const {
        u32 y = reduce_mul(x, 1);
        return y < MOD ? y : y - MOD;
    }

    constexpr ModInt operator+() const { return *this; }
    constexpr ModInt operator-() const { return ModInt() -= *this; }

    constexpr ModInt& operator+=(const ModInt& rhs) {
        x += rhs.x;
        x = std::min(x, x - 2 * MOD);
        return *this;
    }
    constexpr friend ModInt operator+(const ModInt& lhs, const ModInt& rhs) {
        return ModInt(lhs) += rhs;
    }

    constexpr ModInt& operator-=(const ModInt& rhs) {
        x += 2 * MOD - rhs.x;
        x = std::min(x, x - 2 * MOD);
        return *this;
    }
    constexpr friend ModInt operator-(const ModInt& lhs, const ModInt& rhs) {
        return ModInt(lhs) -= rhs;
    }

    constexpr ModInt& operator*=(const ModInt& rhs) {
        x = reduce_mul(x, rhs.x);
        return *this;
    }
    constexpr friend ModInt operator*(const ModInt& lhs, const ModInt& rhs) {
        return ModInt(lhs) *= rhs;
    }

    friend bool operator==(const ModInt& lhs, const ModInt& rhs) {
        auto lx = lhs.x;
        if (lx >= MOD) lx -= MOD;
        auto rx = rhs.x;
        if (rx >= MOD) rx -= MOD;
        return lx == rx;
    }

    constexpr ModInt pow(u64 n) const {
        ModInt v = *this, r = 1;
        while (n) {
            if (n & 1) r *= v;
            v *= v;
            n >>= 1;
        }
        return r;
    }
    constexpr ModInt inv() const {
        // TODO: for non-prime
        return pow(MOD - 2);
    }

  private:
    u32 x;

    static constexpr u32 B = ((u64(1) << 32)) % MOD;
    static constexpr u32 B2 = u64(1) * B * B % MOD;
    static constexpr u32 INV = -inv_u32(MOD);

    // Input: (l * r) must be no more than (2^32 * MOD)
    // Output: ((l * r) / 2^32) % MOD
    static constexpr u32 reduce_mul(u32 l, u32 r) {
        u64 x = u64(1) * l * r;
        x += u64(u32(x) * INV) * MOD;
        return u32(x >> 32);
    }
};

using ModInt998244353 = ModInt<998244353>;
using ModInt1000000007 = ModInt<1000000007>;

}  // namespace yosupo
