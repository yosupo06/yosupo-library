#pragma once

#include "yosupo/modint.hpp"
#include "yosupo/simd/vector.hpp"

namespace yosupo {

// f(x[i]) = (x[i] / (2^32)) (mod m)
// input range: x[i] + (2^32 - 1) * m < 2^63
// output range: (x[i] / 2^32) <= f(x[i]) <= floor(x[i] / 2^32) + m
template <int MOD>
intx8 montgomery_reduction(const llx4& x0246, const llx4& x1357) {
    static_assert(MOD > 0 && MOD % 2, "mod must be positive & odd");

    static constexpr int nim =
        -(int)atcoder::internal::inv_gcd(MOD, 1LL << 32).second;

    __m256i km0246 =
        _mm256_mul_epu32(_mm256_mul_epu32(x0246.raw(), _mm256_set1_epi32(nim)),
                         _mm256_set1_epi32(MOD));
    __m256i km1357 =
        _mm256_mul_epu32(_mm256_mul_epu32(x1357.raw(), _mm256_set1_epi32(nim)),
                         _mm256_set1_epi32(MOD));

    llx4 z0246 = llx4(x0246) + llx4(km0246);
    llx4 z1357 = llx4(x1357) + llx4(km1357);

    return _mm256_blend_epi32(_mm256_shuffle_epi32(z0246.raw(), 0xf5),
                              z1357.raw(), 0b10101010);
}

/*
vectorized modint (by montgomery reduction)
*/
template <int MOD> struct modintx8 {
    static_assert(MOD % 2, "mod must be positive & odd");
    static_assert(1 <= MOD && MOD <= (1 << 30) - 1,
                  "mod range: [1, (1<<30) - 1]");

    using mint = static_modint<MOD>;
    static const int B = ((1LL << 32)) % MOD;
    static const int iB = atcoder::internal::inv_gcd(B, MOD).second;

    // 0 <= d && d <= 2 * mod
    // d[i] = (actual value) * B
    intx8 d;

    modintx8() : d(0) {}
    modintx8(const std::array<mint, 8>& _d) {
        d = intx8(_d[0].val(), _d[1].val(), _d[2].val(), _d[3].val(),
                  _d[4].val(), _d[5].val(), _d[6].val(), _d[7].val());
        (*this) *= modintx8(B);
    }
    modintx8(mint x0,
             mint x1,
             mint x2,
             mint x3,
             mint x4,
             mint x5,
             mint x6,
             mint x7)
        : d(_mm256_set_epi32(x7.val(),
                             x6.val(),
                             x5.val(),
                             x4.val(),
                             x3.val(),
                             x2.val(),
                             x1.val(),
                             x0.val())) {
        (*this) *= modintx8(B);
    }

    modintx8(mint x) : d(int((x * B).val())) {}

    mint at(int i) const {
        return mint(1ULL * d.at(i) * iB);
    }
    void set(int i, mint x) {
        d.set(i, (x * B).val());
    }

    modintx8& operator+=(const modintx8& rhs) {
        d += rhs.d;
        d -= intx8(2 * MOD);
        d += intx8(2 * MOD) & d.sign();
        return *this;
    }
    modintx8& operator-=(const modintx8& rhs) {
        d -= rhs.d;
        d += intx8(2 * MOD) & d.sign();
        return *this;
    }
    modintx8& operator*=(const modintx8& rhs) {
        auto v = d.mul(rhs.d);
        d = montgomery_reduction<mint::mod()>(v.first, v.second);
        return *this;
    }
    friend modintx8 operator+(const modintx8& lhs, const modintx8& rhs) {
        return modintx8(lhs) += rhs;
    }
    friend modintx8 operator-(const modintx8& lhs, const modintx8& rhs) {
        return modintx8(lhs) -= rhs;
    }
    friend modintx8 operator*(const modintx8& lhs, const modintx8& rhs) {
        return modintx8(lhs) *= rhs;
    }
    template <int N> modintx8 neg() const {
        modintx8 w;
        w.d = (d - intx8(_mm256_blend_epi32(_mm256_setzero_si256(),
                                            _mm256_set1_epi32(2 * MOD), N)))
                  .abs();
        return w;
    }

    modintx8 operator-() const {
        return neg<0b11111111>();
    }

    modintx8& clear(unsigned char n) {
        d.clear(n);
        return *this;
    }

    template <int N> modintx8 shuffle() const {
        modintx8 x;
        x.d.d = _mm256_shuffle_epi32(d.d, N);
        return x;
    }
    template <int N> modintx8 shufflex4() const {
        modintx8 x;
        x.d.d = _mm256_permute2x128_si256(d.d, d.d, N);
        return x;
    }

    std::array<mint, 8> to_array() const {
        auto buf = (*this * modintx8(iB)).d;
        buf -= intx8(MOD) & (buf > intx8(MOD - 1));
        auto v = buf.to_array();
        std::array<mint, 8> x;
        for (int i = 0; i < 8; i++) {
            x[i] = mint::raw(v[i]);
        }
        return x;
    }

    static modintx8 from_raw(const intx8& _d) {
        modintx8 x;
        x.d = _d;
        return x;
    }
};
}  // namespace yosupo