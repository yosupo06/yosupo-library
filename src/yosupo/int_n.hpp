#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cctype>
#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>
#include <type_traits>
#include "yosupo/dump.hpp"

namespace yosupo {

template <int N> struct UintN {
    using u64 = uint64_t;
    using u128 = unsigned __int128;

    const std::array<uint64_t, N>& data() const { return d; }

    // Constructor

    UintN() = default;

    template <typename T>
        requires std::is_unsigned_v<T> && (sizeof(T) <= sizeof(uint64_t))
    UintN(T v) {
        d[0] = v;
    }

    template <typename T>
        requires std::is_signed_v<T> && (sizeof(T) <= sizeof(uint64_t))
    UintN(T v) : UintN(static_cast<uint64_t>(v)) {
        if (v < 0) {
            std::fill(d.begin() + 1, d.end(), -1);
        }
    }

    UintN(u128 v)
        requires(N >= 2)
    {
        d[0] = (u64)v;
        if constexpr (N > 1) d[1] = (u64)(v >> 64);
    }
    UintN(__int128 v)
        requires(N >= 2)
        : UintN(static_cast<u128>(v)) {
        if (N > 2 && v < 0) {
            std::fill(d.begin() + 2, d.end(), -1);
        }
    }

    template <int M>
        requires(!(M > N))
    UintN(const UintN<M>& other) {
        for (int i = 0; i < M; i++) {
            d[i] = other.data()[i];
        }
        for (int i = M; i < N; i++) {
            d[i] = 0;
        }
    }
    template <int M>
        requires(M > N)
    explicit UintN(const UintN<M>& other) {
        for (int i = 0; i < N; i++) {
            d[i] = other.data()[i];
        }
    }

    explicit UintN(const std::string& s) {
        if (s[0] == '-') {
            *this = -UintN(s.substr(1));
            return;
        }
        UintN ten(10);
        for (char c : s) {
            assert(isdigit(c));
            *this = *this * ten + UintN(uint64_t(c - '0'));
        }
    }

    // Compare

    bool operator==(const UintN& rhs) const { return d == rhs.d; }
    bool operator!=(const UintN& rhs) const { return d != rhs.d; }
    std::strong_ordering operator<=>(const UintN& rhs) const {
        for (int i = N - 1; i >= 0; i--) {
            if (d[i] != rhs.d[i]) return d[i] <=> rhs.d[i];
        }
        return std::strong_ordering::equal;
    }
    explicit operator bool() const { return *this != UintN(0); }

    template <std::integral T> explicit operator T() const {
        return T(data()[0]);
    }

    // Arithmetic

    UintN operator-() const {
        UintN res;
        bool carry = true;
        for (int i = 0; i < N; i++) {
            carry = __builtin_add_overflow(~d[i], carry, &res.d[i]);
        }
        return res;
    }

    UintN operator+(const UintN& rhs) const {
        UintN res;
        bool carry = false;
        for (int i = 0; i < N; i++) {
            bool carry2 = __builtin_add_overflow(d[i], rhs.d[i], &res.d[i]);
            if (carry) carry2 |= __builtin_add_overflow(res.d[i], 1, &res.d[i]);
            carry = carry2;
        }
        return res;
    }
    UintN& operator+=(const UintN& rhs) { return *this = *this + rhs; }

    UintN operator-(const UintN& rhs) const {
        UintN res;
        bool carry = false;
        for (int i = 0; i < N; i++) {
            bool carry2 = __builtin_sub_overflow(d[i], rhs.d[i], &res.d[i]);
            if (carry) carry2 |= __builtin_sub_overflow(res.d[i], 1, &res.d[i]);
            carry = carry2;
        }
        return res;
    }
    UintN& operator-=(const UintN& rhs) { return *this = *this - rhs; }

    UintN operator*(const UintN& rhs) const {
        UintN res;
        for (int i = 0; i < N; i++) {
            u64 carry = 0;
            for (int j = 0; i + j < N; j++) {
                u128 prod = (u128)d[i] * rhs.d[j] + res.d[i + j] + carry;
                res.d[i + j] = static_cast<u64>(prod);
                carry = static_cast<u64>(prod >> 64);
            }
        }
        return res;
    }
    UintN& operator*=(const UintN& rhs) { return *this = *this * rhs; }

    std::pair<UintN, u64> divrem(const u64& rhs) const {
        u128 r = 0;
        UintN q;
        for (int i = N - 1; i >= 0; i--) {
            auto qr = div128((r << 64) | d[i], rhs);
            q.d[i] = qr.first;
            r = qr.second;
        }
        return {q, r};
    }

    std::pair<UintN, UintN> divrem(const UintN& rhs) const {
        assert(rhs);

        int width = rhs.bit_width();
        int block = width / 64;
        int shift = 63 - width % 64;

        UintN<N + 1> l = *this, r = rhs;
        if (shift) {
            l = l << shift;
            r = r << shift;
        }
        const u64 r_top = r.data()[block];

        UintN<N> q;
        r <<= (N - 1 - block) * 64;
        for (int i = N - 1 - block; i >= 0; i--) {
            const u128 l_top =
                (u128)l.data()[i + block + 1] << 64 | l.data()[i + block];
            u64 rough = (r_top == std::numeric_limits<u64>::max())
                            ? (u64)(l_top >> 64)
                            : div128(l_top, r_top + 1).first;
            q.d[i] = rough;
            l -= r * rough;
            while (l >= r) {
                l -= r;
                q.d[i]++;
            }
            r >>= 64;
        }
        if (shift) {
            l >>= shift;
        }
        return {q, UintN(l)};
    }

    UintN operator/(const UintN& rhs) const { return this->divrem(rhs).first; }
    UintN& operator/=(const UintN& rhs) { return *this = *this / rhs; }

    UintN operator%(const UintN& rhs) const { return this->divrem(rhs).second; }
    UintN& operator%=(const UintN& rhs) { return *this = *this % rhs; }

    template <typename T>
        requires std::is_unsigned_v<T> && (sizeof(T) <= sizeof(uint64_t))
    UintN operator/(T v) const {
        return this->divrem((u64)v).first;
    }
    template <typename T>
        requires std::is_unsigned_v<T> && (sizeof(T) <= sizeof(uint64_t))
    UintN operator%(T v) const {
        return this->divrem((u64)v).second;
    }

    // Bitop

    UintN operator<<(int shift) const {
        if (shift >= 64 * N) return UintN();
        assert(shift >= 0);
        UintN res;
        int s = shift / 64, t = shift % 64;
        for (int i = N - 1; i >= s; i--) {
            res.d[i] |= d[i - s] << t;
            if (t && i - s > 0) res.d[i] |= d[i - s - 1] >> (64 - t);
        }
        return res;
    }
    UintN& operator<<=(int shift) { return *this = *this << shift; }

    UintN operator>>(int shift) const {
        if (shift >= 64 * N) return UintN();
        assert(shift >= 0);
        UintN res;
        int s = shift / 64, t = shift % 64;
        for (int i = 0; i < N - s; i++) {
            res.d[i] |= d[i + s] >> t;
            if (t && i + s + 1 < N) res.d[i] |= d[i + s + 1] << (64 - t);
        }
        return res;
    }
    UintN& operator>>=(int shift) { return *this = *this >> shift; }

    int bit_width() const {
        for (int i = N - 1; i >= 0; i--) {
            if (d[i] == 0) continue;
            return (int)std::bit_width(d[i]) + i * 64;
        }
        return 0;
    }

    int countr_zero() const {
        for (int i = 0; i < N; i++) {
            if (d[i] == 0) continue;
            return (int)std::countr_zero(d[i]) + i * 64;
        }
        return 64 * N;
    }

    static UintN gcd(UintN a, UintN b) {
        if (a == 0) return b;
        if (b == 0) return a;
        int shift;
        {
            int a_bsf = a.countr_zero();
            a >>= a_bsf;
            int b_bsf = b.countr_zero();
            b >>= b_bsf;
            shift = std::min(a_bsf, b_bsf);
        }
        while (a != b) {
            if (a > b) std::swap(a, b);
            b -= a;
            b >>= b.countr_zero();
        }
        return (a << shift);
    }

    std::string dump() const {
        auto x = *this;
        if (!x) {
            return "0";
        }
        std::string s;
        while (x) {
            auto [q, r] = x.divrem(10U);
            x = q;
            s.push_back(char('0' + r));
        }
        std::reverse(s.begin(), s.end());
        return s;
    }

  private:
    std::array<uint64_t, N> d = {{}};

    static std::pair<u64, u64> div128(u128 a, u64 b) {
        u128 q = a / b;
        if (q > std::numeric_limits<u64>::max()) {
            __builtin_unreachable();
        }
        u64 r = (u64)(a % b);
        return {(u64)q, r};
    }
};

template <int N> struct IntN {
    const std::array<uint64_t, N>& data() const { return d.data(); }

    IntN() = default;
    IntN(UintN<N> _d) : d(_d) {}

    template <typename T>
        requires std::is_integral_v<T>
    IntN(T v) : d(v) {}

    explicit IntN(const std::string& s) : d(s) {}
    operator UintN<N>() const { return d; }
    template <std::integral T> explicit operator T() const { return T(d); }

    bool operator==(const IntN& rhs) const { return d == rhs.d; }
    bool operator!=(const IntN& rhs) const { return d != rhs.d; }
    std::strong_ordering operator<=>(const IntN& rhs) const {
        if (data()[N - 1] != rhs.data()[N - 1]) {
            const uint64_t sign = 1ULL << 63;
            return (data()[N - 1] ^ sign) <=> (rhs.data()[N - 1] ^ sign);
        }
        for (int i = N - 2; i >= 0; i--) {
            if (data()[i] != rhs.data()[i]) return data()[i] <=> rhs.data()[i];
        }
        return std::strong_ordering::equal;
    }
    explicit operator bool() const { return (bool)this->d; }

    IntN operator-() const { return IntN(-d); }
    IntN operator+(const IntN& rhs) const { return IntN(d + rhs.d); }
    IntN& operator+=(const IntN& rhs) { return *this = *this + rhs; }
    IntN operator-(const IntN& rhs) const { return IntN(d - rhs.d); }
    IntN& operator-=(const IntN& rhs) { return *this = *this - rhs; }
    IntN operator*(const IntN& rhs) const {
        UintN<N> absL = abs();
        UintN<N> absR = rhs.abs();
        UintN<N> prod = absL * absR;
        if (is_negative() ^ rhs.is_negative())
            return IntN(-prod);
        else
            return IntN(prod);
    }
    IntN& operator*=(const IntN& rhs) { return *this = *this * rhs; }

    IntN operator/(const IntN& rhs) const {
        UintN<N> absL = abs();
        UintN<N> absR = rhs.abs();
        UintN<N> q = absL / absR;
        if (is_negative() ^ rhs.is_negative())
            return IntN(-q);
        else
            return IntN(q);
    }
    IntN& operator/=(const IntN& rhs) { return *this = *this / rhs; }

    IntN operator%(const IntN& rhs) const {
        UintN<N> absL = abs();
        UintN<N> absR = rhs.abs();
        UintN<N> r = absL % absR;
        if (is_negative())
            return IntN(-r);
        else
            return IntN(r);
    }
    IntN& operator%=(const IntN& rhs) { return *this = *this % rhs; }

    IntN operator<<(int shift) const { return IntN(d << shift); }

    bool is_negative() const { return data()[N - 1] >> 63; }
    int sgn() const { return is_negative() ? -1 : (*this ? 1 : 0); }

    IntN abs() const { return is_negative() ? -*this : *this; }

    static IntN gcd(IntN a, IntN b) {
        return IntN(UintN<N>::gcd(a.abs(), b.abs()));
    }

    std::string dump() const {
        if (is_negative()) {
            return "-" + static_cast<UintN<N>>(-*this).dump();
        }
        return static_cast<UintN<N>>(*this).dump();
    }

  private:
    UintN<N> d;
};

}  // namespace yosupo
