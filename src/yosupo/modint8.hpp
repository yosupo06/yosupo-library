#pragma once

#include <immintrin.h>
#include <span>

#include "yosupo/math.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <i32 MOD> struct ModInt8 {
    using modint = ModInt<MOD>;
    static_assert(sizeof(modint) == 4);

    using m256i_u = __m256i_u;
    static constexpr i32 mod() { return MOD; }

    __attribute__((target("avx2"))) ModInt8() : x(_mm256_setzero_si256()) {}

    __attribute__((target("avx2"))) ModInt8(modint* _x)
        : x(_mm256_loadu_si256((m256i_u*)_x)) {}
    __attribute__((target("avx2"))) ModInt8(std::span<const modint, 8> _x)
        : x(_mm256_loadu_si256((m256i_u*)_x.data())) {
        static_assert(sizeof(modint) == 4);
    }

    __attribute__((target("avx2"))) ModInt8(modint x0,
                                            modint x1,
                                            modint x2,
                                            modint x3,
                                            modint x4,
                                            modint x5,
                                            modint x6,
                                            modint x7)
        : ModInt8(std::array<modint, 8>{x0, x1, x2, x3, x4, x5, x6, x7}) {}

    __attribute__((target("avx2"))) static ModInt8 set1(modint x) {
        ModInt8 v;
        v.x = _mm256_set1_epi32(*((int*)&x));
        return v;
    }

    __attribute__((target("avx2"))) std::array<u32, 8> val() const {
        auto a = mul(x, _mm256_set1_epi32(1));
        alignas(32) std::array<u32, 8> b;
        _mm256_storeu_si256((__m256i_u*)b.data(),
                            min(a, _mm256_sub_epi32(a, MOD_X())));
        return b;
    }

    __attribute__((target("avx2"))) ModInt8& operator+=(const ModInt8& rhs) {
        x = _mm256_add_epi32(x, rhs.x);
        x = min(x, _mm256_sub_epi32(x, MOD2_X()));
        return *this;
    }
    __attribute__((target("avx2"))) friend ModInt8 operator+(
        const ModInt8& lhs,
        const ModInt8& rhs) {
        return ModInt8(lhs) += rhs;
    }

    __attribute__((target("avx2"))) ModInt8& operator-=(const ModInt8& rhs) {
        x = _mm256_sub_epi32(x, rhs.x);
        x = min(x, _mm256_add_epi32(x, MOD2_X()));
        return *this;
    }
    __attribute__((target("avx2"))) friend ModInt8 operator-(
        const ModInt8& lhs,
        const ModInt8& rhs) {
        return ModInt8(lhs) -= rhs;
    }

    __attribute__((target("avx2"))) ModInt8& operator*=(const ModInt8& rhs) {
        x = mul(x, rhs.x);
        return *this;
    }
    __attribute__((target("avx2"))) friend ModInt8 operator*(
        const ModInt8& lhs,
        const ModInt8& rhs) {
        return ModInt8(lhs) *= rhs;
    }

    __attribute__((target("avx2"))) ModInt8 operator-() const {
        return ModInt8() - *this;
    }

    __attribute__((target("avx2"))) friend bool operator==(const ModInt8& lhs,
                                                           const ModInt8& rhs) {
        auto lx = lhs.x, rx = rhs.x;
        lx = min(lx, _mm256_sub_epi32(lx, MOD_X()));
        rx = min(rx, _mm256_sub_epi32(rx, MOD_X()));
        auto z = _mm256_xor_si256(lx, rx);
        return _mm256_testz_si256(z, z);
    }

    // a.permutevar(idx)[i] = a[idx[i] % 8]
    __attribute__((target("avx2"))) ModInt8
    permutevar(const std::array<u32, 8>& idx) const {
        return permutevar(_mm256_loadu_si256((const m256i_u*)idx.data()));
    }

    // a[i] <- a[(middle + i) % 8]
    __attribute__((target("avx2"))) ModInt8 rotate(u32 middle) const {
        static const m256i_u base = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
        return permutevar(_mm256_add_epi32(base, _mm256_set1_epi32(middle)));
    }

    template <uint8_t MASK>
    __attribute__((target("avx2"))) friend ModInt8 blend(const ModInt8& lhs,
                                                         const ModInt8& rhs) {
        ModInt8 v;
        v.x = _mm256_blend_epi32(lhs.x, rhs.x, MASK);
        return v;
    }

    __attribute__((target("avx2"))) friend ModInt8 blendvar(
        const ModInt8& lhs,
        const ModInt8& rhs,
        const std::array<u32, 8>& idx) {
        ModInt8 v;
        v.x = _mm256_blendv_epi8(
            rhs.x, lhs.x,
            _mm256_cmpeq_epi32(_mm256_loadu_si256((const m256i_u*)idx.data()),
                               _mm256_setzero_si256()));
        return v;
    }

  private:
    m256i_u x;

    __attribute__((target("avx2"))) static m256i_u MOD_X() {
        return _mm256_set1_epi32(MOD);
    }
    __attribute__((target("avx2"))) static m256i_u MOD2_X() {
        return _mm256_set1_epi32(2 * MOD);
    }
    __attribute__((target("avx2"))) static m256i_u N_INV_X() {
        return _mm256_set1_epi32(-inv_u32(MOD));
    }
    __attribute__((target("avx2"))) static m256i_u INT_MIN_X() {
        return _mm256_set1_epi32(u32(1) << 31);
    }

    static constexpr u32 B2 = pow_mod_u64(2, 64, MOD);

    // Input: l * r <= 2^32 * MOD
    // Output: l * r >>= 2^32
    __attribute__((target("avx2"))) static m256i_u mul(const m256i_u& l,
                                                       const m256i_u& r) {
        auto x0 = mul_even(l, r);
        auto x1 = mul_even(_mm256_shuffle_epi32(l, 0xf5),
                           _mm256_shuffle_epi32(r, 0xf5));
        x0 += mul_even(mul_even(x0, N_INV_X()), MOD_X());
        x1 += mul_even(mul_even(x1, N_INV_X()), MOD_X());

        x0 = _mm256_srli_epi64(x0, 32);
        return _mm256_blend_epi32(x0, x1, 0b10101010);
    }
    // (lr[0], lr[2], lr[4], lr[6])
    __attribute__((target("avx2"))) static m256i_u mul_even(const m256i_u& l,
                                                            const m256i_u& r) {
        return _mm256_mul_epu32(l, r);
    }

    __attribute__((target("avx2"))) static m256i_u min(const m256i_u& l,
                                                       const m256i_u& r) {
        return _mm256_min_epu32(l, r);
    }
    __attribute__((target("avx2"))) static m256i_u max(const m256i_u& l,
                                                       const m256i_u& r) {
        return _mm256_max_epu32(l, r);
    }

    // a.permutevar(idx)[i] = a[idx[i] % 8]
    __attribute__((target("avx2"))) ModInt8
    permutevar(const m256i_u& idx) const {
        ModInt8 v;
        v.x = _mm256_permutevar8x32_epi32(x, idx);
        return v;
    }
};

}  // namespace yosupo