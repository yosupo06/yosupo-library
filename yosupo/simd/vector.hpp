#pragma once

static_assert(__cplusplus >= 201703L, "C++17 or later");

#include <immintrin.h>
#include <array>

namespace yosupo {

struct llx4 {
  public:
    llx4() : d() {}
    llx4(long long x) : d(_mm256_set1_epi64x(x)) {}
    llx4(const __m256i& x) : d(x) {}
    llx4(const std::array<long long, 8>& x)
        : d(_mm256_loadu_si256((__m256i*)x.data())) {}
    llx4(long long x0, long long x1, long long x2, long long x3)
        : d(_mm256_set_epi64x(x3, x2, x1, x0)) {}

    std::array<long long, 4> to_array() const {
        alignas(32) std::array<long long, 4> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b;
    }
    long long at(int i) const {
        alignas(32) std::array<long long, 4> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b[i];
    }
    void set(int i, long long x) {
        alignas(32) std::array<long long, 4> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        b[i] = x;
        d = _mm256_load_si256((__m256i*)b.data());
    }

    llx4& operator+=(const llx4& rhs) {
        d = _mm256_add_epi64(d, rhs.d);
        return *this;
    }
    friend llx4 operator+(const llx4& lhs, const llx4& rhs) {
        return llx4(lhs) += rhs;
    }
    llx4& operator-=(const llx4& rhs) {
        d = _mm256_sub_epi64(d, rhs.d);
        return *this;
    }
    friend llx4 operator-(const llx4& lhs, const llx4& rhs) {
        return llx4(lhs) -= rhs;
    }
    __m256i raw() const { return d; }

    __m256i d;
};

struct intx8 {
  public:
    intx8() : d() {}
    intx8(int x) : d(_mm256_set1_epi32(x)) {}
    intx8(const __m256i& x) : d(x) {}
    intx8(const std::array<int, 8>& x)
        : d(_mm256_loadu_si256((__m256i*)x.data())) {}
    intx8(int x0, int x1, int x2, int x3, int x4, int x5, int x6, int x7)
        : d(_mm256_set_epi32(x7, x6, x5, x4, x3, x2, x1, x0)) {}

    std::array<int, 8> to_array() const {
        alignas(32) std::array<int, 8> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b;
    }
    int at(int i) const {
        alignas(32) std::array<int, 8> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        return b[i];
    }
    void set(int i, int x) {
        alignas(32) std::array<int, 8> b;
        _mm256_store_si256((__m256i*)b.data(), d);
        b[i] = x;
        d = _mm256_load_si256((__m256i*)b.data());
    }

    intx8& operator+=(const intx8& rhs) {
        d = _mm256_add_epi32(d, rhs.d);
        return *this;
    }
    friend intx8 operator+(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) += rhs;
    }
    intx8& operator-=(const intx8& rhs) {
        d = _mm256_sub_epi32(d, rhs.d);
        return *this;
    }
    friend intx8 operator-(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) -= rhs;
    }

    // return (0246, 1357)
    std::pair<llx4, llx4> mul(const intx8 rhs) const {
        __m256i x0246 = _mm256_mul_epi32(d, rhs.d);
        __m256i x1357 = _mm256_mul_epi32(_mm256_shuffle_epi32(d, 0xf5),
                                         _mm256_shuffle_epi32(rhs.d, 0xf5));
        return {x0246, x1357};
    }

    intx8& operator&=(const intx8& rhs) {
        d = _mm256_and_si256(d, rhs.d);
        return *this;
    }
    friend intx8 operator&(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) &= rhs;
    }

    // d[i] <<= r[i] (not mod 32)
    intx8 operator<<=(const intx8& rhs) {
        d = _mm256_sllv_epi32(d, rhs.d);
        return *this;
    }
    friend intx8 operator<<(const intx8& lhs, const intx8& rhs) {
        return intx8(lhs) <<= rhs;
    }

    // (d[i] > rhs[i] ? -1 : 0), -1 means that all bit set
    intx8 operator>(const intx8& rhs) const {
        return _mm256_cmpgt_epi32(d, rhs.d);
    }
    intx8 operator<(const intx8& rhs) const { return rhs > *this; }

    bool test_all_zero() const { return _mm256_testz_si256(d, d) == 1; }

    // (d[i] < 0 ? -1 : 0), -1 means that all bit set
    intx8 sign() const { return *this < intx8(_mm256_setzero_si256()); }

    intx8 abs() const { return intx8(_mm256_abs_epi32(d)); }

    // d[i] = ((n & (1 << i)) ? 0 : d[i])
    intx8 clear(unsigned char n) {
        intx8 mask = intx8(n) << intx8(31, 30, 29, 28, 27, 26, 25, 24);        
        d = _mm256_andnot_si256(_mm256_srai_epi32(mask.d, 31), d);
        return *this;        
    }

    // return (0246, 1357)
    std::pair<llx4, llx4> split() const {
        return {
            llx4(((*this) & intx8(-1, 0, -1, 0, -1, 0, -1, 0)).d),
            llx4(_mm256_srli_epi64(d, 32)),
        };
    }
    __m256i raw() const { return d; }

    __m256i d;
};

}  // namespace yosupo
