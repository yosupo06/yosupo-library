#pragma once

#include <immintrin.h>
#include <xmmintrin.h>

#include <array>
#include <span>
#include <string>

#include "yosupo/dump.hpp"
#include "yosupo/math.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <i32 MOD> struct ModInt8 {
  private:
    using modint = ModInt<MOD>;
    static_assert(sizeof(modint) == 4);

    alignas(32) std::array<modint, 8> x;

    using m256i_u = __m256i_u;
    __attribute__((target("avx2"))) m256i_u to_m256i_u() const {
        return _mm256_loadu_si256((m256i_u*)x.data());
    }
    __attribute__((target("avx2"))) explicit ModInt8(m256i_u _x) {
        _mm256_storeu_si256((m256i_u*)x.data(), _x);
    }

  public:
    __attribute__((target("avx2"))) ModInt8() : x() {}
    __attribute__((target("avx2"))) explicit ModInt8(modint _x) { x.fill(_x); }

    __attribute__((target("avx2"))) ModInt8(std::span<const modint, 8> _x) {
        std::copy_n(_x.begin(), 8, x.begin());
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

    __attribute__((target("avx2"))) std::array<u32, 8> val() const {
        auto _x = to_m256i_u();
        auto a = reduce_mul(_x, _mm256_set1_epi32(1));
        alignas(32) std::array<u32, 8> b;
        _mm256_storeu_si256((__m256i_u*)b.data(),
                            min(a, _mm256_sub_epi32(a, MOD_X())));
        return b;
    }

    __attribute__((target("avx2"))) std::array<modint, 8> to_array() const {
        return x;
    }

    __attribute__((target("avx2"))) friend ModInt8 operator+(
        const ModInt8& lhs,
        const ModInt8& rhs) {
        auto _x = _mm256_add_epi32(lhs.to_m256i_u(), rhs.to_m256i_u());
        return ModInt8(min(_x, _mm256_sub_epi32(_x, MOD2_X())));
    }
    __attribute__((target("avx2"))) ModInt8& operator+=(const ModInt8& rhs) {
        return *this = *this + rhs;
    }

    __attribute__((target("avx2"))) friend ModInt8 operator-(
        const ModInt8& lhs,
        const ModInt8& rhs) {
        auto _x = _mm256_sub_epi32(lhs.to_m256i_u(), rhs.to_m256i_u());
        return ModInt8(min(_x, _mm256_add_epi32(_x, MOD2_X())));
    }
    __attribute__((target("avx2"))) ModInt8& operator-=(const ModInt8& rhs) {
        return *this = *this - rhs;
    }

    __attribute__((target("avx2"))) friend ModInt8 operator*(
        const ModInt8& lhs,
        const ModInt8& rhs) {
        return ModInt8(reduce_mul(lhs.to_m256i_u(), rhs.to_m256i_u()));
    }

    __attribute__((target("avx2"))) ModInt8& operator*=(const ModInt8& rhs) {
        return *this = *this * rhs;
    }

    __attribute__((target("avx2"))) ModInt8 operator-() const {
        return ModInt8() - *this;
    }

    __attribute__((target("avx2"))) friend bool operator==(const ModInt8& lhs,
                                                           const ModInt8& rhs) {
        auto _x = lhs.to_m256i_u();
        auto _y = rhs.to_m256i_u();
        _x = min(_x, _mm256_sub_epi32(_x, MOD_X()));
        _y = min(_y, _mm256_sub_epi32(_y, MOD_X()));
        auto z = _mm256_xor_si256(_x, _y);
        return _mm256_testz_si256(z, z);
    }

    // a.permutevar(idx)[i] = a[idx[i] % 8]
    __attribute__((target("avx2"))) ModInt8
    permutevar(const std::array<u32, 8>& idx) const {
        return ModInt8(_mm256_permutevar8x32_epi32(
            to_m256i_u(), _mm256_loadu_si256((const m256i_u*)idx.data())));
    }

    template <u8 MASK>
    __attribute__((target("avx2"))) friend ModInt8 blend(const ModInt8& lhs,
                                                         const ModInt8& rhs) {
        return ModInt8(
            _mm256_blend_epi32(lhs.to_m256i_u(), rhs.to_m256i_u(), MASK));
    }

    __attribute__((target("avx2"))) std::string dump() const {
        return yosupo::dump(val());
    }

  private:
    static constexpr u32 B2 = pow_mod_u64(2, 64, MOD);
    static constexpr u32 INV = -inv_u32(MOD);

    __attribute__((target("avx2"))) static m256i_u MOD_X() {
        return _mm256_set1_epi32(MOD);
    }
    __attribute__((target("avx2"))) static m256i_u MOD2_X() {
        return _mm256_set1_epi32(2 * MOD);
    }
    __attribute__((target("avx2"))) static m256i_u N_INV_X() {
        return _mm256_set1_epi32(INV);
    }

    // Input: l * r <= 2^32 * MOD
    // Output: l * r >>= 2^32
    __attribute__((target("avx2"))) static m256i_u reduce_mul(
        const m256i_u& l,
        const m256i_u& r) {
        auto x0 = mul_even(l, r);
        auto x1 = mul_even(_mm256_shuffle_epi32(l, 0xf5),
                           _mm256_shuffle_epi32(r, 0xf5));
        x0 += mul_even(mul_even(x0, N_INV_X()), MOD_X());
        x1 += mul_even(mul_even(x1, N_INV_X()), MOD_X());

        return _mm256_blend_epi32(_mm256_srli_epi64(x0, 32), x1, 0b10101010);
    }

    /* SIMD utils */
    __attribute__((target("avx2"))) static __m256i_u min(const __m256i_u& l,
                                                         const __m256i_u& r) {
        return _mm256_min_epu32(l, r);
    }
    __attribute__((target("avx2"))) static __m256i_u max(const __m256i_u& l,
                                                         const __m256i_u& r) {
        return _mm256_max_epu32(l, r);
    }
    // (lr[0], lr[2], lr[4], lr[6])
    __attribute__((target("avx2"))) static __m256i_u mul_even(
        const __m256i_u& l,
        const __m256i_u& r) {
        return _mm256_mul_epu32(l, r);
    }
};

}  // namespace yosupo