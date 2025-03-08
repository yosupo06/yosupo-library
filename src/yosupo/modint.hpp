#pragma once

#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "yosupo/math.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <uint32_t MOD> struct ModInt {
    static_assert(MOD % 2 && MOD <= (1U << 30) - 1,
                  "mod must be odd and at most 2^30 - 1");

    static constexpr uint32_t mod() { return MOD; }

    constexpr ModInt() : x(0) {}
    constexpr explicit ModInt(uint32_t _x) : x(mulreduce(_x, B2)) {}

    constexpr ModInt(std::signed_integral auto _x)
        : ModInt((uint32_t)(_x % (int32_t)MOD + MOD)) {}
    constexpr ModInt(std::unsigned_integral auto _x)
        : ModInt((uint32_t)(_x % MOD)) {}

    constexpr uint32_t val() const {
        uint32_t y = mulreduce(x, 1);
        return y < MOD ? y : y - MOD;
    }
    constexpr uint32_t internal_val() const { return x; }

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
        x = mulreduce(x, rhs.x);
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

    constexpr ModInt pow(uint64_t n) const {
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

    friend std::ostream& operator<<(std::ostream& os, const ModInt& v) {
        return os << v.val();
    }

  private:
    uint32_t x;

    static constexpr uint32_t B = ((uint64_t(1) << 32)) % MOD;
    static constexpr uint32_t B2 = uint64_t(1) * B * B % MOD;
    static constexpr uint32_t INV = -inv_u32(MOD);

    // Input: (l * r) must be no more than (2^32 * MOD)
    // Output: ((l * r) / 2^32) % MOD
    static constexpr uint32_t mulreduce(uint32_t l, uint32_t r) {
        uint64_t x = uint64_t(1) * l * r;
        x += uint64_t(uint32_t(x) * INV) * MOD;
        return uint32_t(x >> 32);
    }
};

using ModInt998244353 = ModInt<998244353>;
using ModInt1000000007 = ModInt<1000000007>;

}  // namespace yosupo
