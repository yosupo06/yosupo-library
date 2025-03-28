#pragma once

#include <algorithm>
#include <concepts>
#include <cstdlib>
#include <string>

#include "yosupo/math/basic.hpp"
#include "yosupo/math/prime.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <i32 MOD>
    requires requires {
        MOD % 2 == 1 && 1 <= MOD&& MOD <= (1 << 30) - 1;
        is_prime(MOD);
    }
struct ModInt {
    static constexpr i32 mod() { return MOD; }

    constexpr ModInt() : x(0) {}

    // It's OK because _x * B2 < 2**32 * MOD
    constexpr ModInt(u32 _x) : x(reduce_mul(_x, B2)) {}

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

    constexpr ModInt& operator/=(const ModInt& rhs) {
        return *this *= rhs.inv();
    }
    constexpr friend ModInt operator/(const ModInt& lhs, const ModInt& rhs) {
        return ModInt(lhs) /= rhs;
    }

    friend bool operator==(const ModInt& lhs, const ModInt& rhs) {
        return std::min(lhs.x, lhs.x - MOD) == std::min(rhs.x, rhs.x - MOD);
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
    constexpr ModInt inv() const { return pow(MOD - 2); }

    std::string dump() const { return std::to_string(val()); }

  private:
    u32 x;  // [0, 2 * MOD)

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
