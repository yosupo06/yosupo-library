#pragma once

#include <algorithm>
#include <array>
#include <span>
#include <string>

#include "yosupo/dump.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <i32 MOD> struct ModInt8 {
  private:
    using modint = ModInt<MOD>;
    static_assert(sizeof(modint) == 4);

    alignas(32) std::array<modint, 8> x;

  public:
    ModInt8() : x() {}
    explicit ModInt8(modint _x) { x.fill(_x); }

    ModInt8(std::span<const modint, 8> _x) {
        std::copy_n(_x.begin(), 8, x.begin());
    }
    ModInt8(modint x0,
            modint x1,
            modint x2,
            modint x3,
            modint x4,
            modint x5,
            modint x6,
            modint x7)
        : ModInt8(std::array<modint, 8>{x0, x1, x2, x3, x4, x5, x6, x7}) {}

    std::array<u32, 8> val() const {
        std::array<u32, 8> y;
        for (int i = 0; i < 8; i++) y[i] = x[i].val();
        return y;
    }

    std::array<modint, 8> to_array() const { return x; }

    friend ModInt8 operator+(const ModInt8& lhs, const ModInt8& rhs) {
        ModInt8 y;
        for (int i = 0; i < 8; i++) y.x[i] = lhs.x[i] + rhs.x[i];
        return y;
    }
    ModInt8& operator+=(const ModInt8& rhs) { return *this = *this + rhs; }

    friend ModInt8 operator-(const ModInt8& lhs, const ModInt8& rhs) {
        ModInt8 y;
        for (int i = 0; i < 8; i++) y.x[i] = lhs.x[i] - rhs.x[i];
        return y;
    }
    ModInt8& operator-=(const ModInt8& rhs) { return *this = *this - rhs; }

    friend ModInt8 operator*(const ModInt8& lhs, const ModInt8& rhs) {
        ModInt8 y;
        for (int i = 0; i < 8; i++) y.x[i] = lhs.x[i] * rhs.x[i];
        return y;
    }
    ModInt8& operator*=(const ModInt8& rhs) { return *this = *this * rhs; }

    ModInt8 operator-() const { return ModInt8() - *this; }

    friend bool operator==(const ModInt8& lhs, const ModInt8& rhs) {
        return lhs.x == rhs.x;
    }

    // a.permutevar(idx)[i] = a[idx[i] % 8]
    ModInt8 permutevar(const std::array<u32, 8>& idx) const {
        ModInt8 y;
        for (int i = 0; i < 8; i++) y.x[i] = x[idx[i] & 7];
        return y;
    }

    template <u8 MASK>
    friend ModInt8 blend(const ModInt8& lhs, const ModInt8& rhs) {
        ModInt8 y;
        for (int i = 0; i < 8; i++) {
            y.x[i] = ((MASK >> i) & 1) ? rhs.x[i] : lhs.x[i];
        }
        return y;
    }

    std::string dump() const { return yosupo::dump(val()); }
};

}  // namespace yosupo
