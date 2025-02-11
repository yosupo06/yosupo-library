#pragma once
#include <algorithm>
#include <cassert>
#include <cctype>
#include <compare>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace yosupo {

template <int N> struct UintN {
  private:
    std::array<uint64_t, N> d = {{}};

  public:
    const std::array<uint64_t, N>& data() const { return d; }

  public:
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

    UintN(unsigned __int128 v) {
        d[0] = (uint64_t)v;
        if constexpr (N > 1) d[1] = v >> 64;
    }
    UintN(__int128 v) : UintN(static_cast<unsigned __int128>(v)) {
        if (N > 2 && v < 0) {
            std::fill(d.begin() + 2, d.end(), -1);
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
            uint64_t carry = 0;
            for (int j = 0; i + j < N; j++) {
                unsigned __int128 prod =
                    (unsigned __int128)d[i] * rhs.d[j] + res.d[i + j] + carry;
                res.d[i + j] = static_cast<uint64_t>(prod);
                carry = static_cast<uint64_t>(prod >> 64);
            }
        }
        return res;
    }
    UintN& operator*=(const UintN& rhs) { return *this = *this * rhs; }

    // NOTE: broken
    std::pair<UintN, UintN> divrem(const UintN& rhs) const {
        if (rhs == UintN()) throw std::runtime_error("Division by zero");
        UintN u = *this, v = rhs;
        UintN q;
        int n = N;
        while (n > 0 && v.d[n - 1] == 0) n--;
        int m = N;
        while (m > 0 && u.d[m - 1] == 0) m--;
        if (m < n) return {q, *this};

        int shift = 0;
        {
            uint64_t high = v.d[n - 1];
            while ((high >> 63) == 0) {
                high <<= 1;
                shift++;
            }
        }
        u = u << shift;
        v = v << shift;

        std::array<uint64_t, N + 1> U = {};
        for (int i = 0; i < m; i++) U[i] = u.d[i];
        U[m] = 0;

        for (int j = m - n; j >= 0; j--) {
            __uint128_t numerator =
                ((__uint128_t)U[j + n] << 64) | U[j + n - 1];
            uint64_t qhat = static_cast<uint64_t>(numerator / v.d[n - 1]);
            uint64_t rhat = static_cast<uint64_t>(numerator % v.d[n - 1]);
            while (n > 1 && qhat * v.d[n - 2] >
                                (((__uint128_t)rhat << 64) + U[j + n - 2])) {
                qhat--;
                rhat += v.d[n - 1];
                if (rhat < v.d[n - 1]) break;
            }
            __uint128_t borrow = 0;
            for (int i = 0; i < n; i++) {
                __uint128_t prod = (__uint128_t)qhat * v.d[i];
                __uint128_t sub = U[i + j] - (uint64_t)prod - borrow;
                U[i + j] = static_cast<uint64_t>(sub);
                borrow = (prod >> 64) + ((sub >> 64) & 1);
            }
            U[j + n] -= (uint64_t)borrow;
            q.d[j] = qhat;
            if (U[j + n] > (((uint64_t)1 << 63))) {
                q.d[j]--;
                __uint128_t carry = 0;
                for (int i = 0; i < n; i++) {
                    __uint128_t sum = (__uint128_t)U[i + j] + v.d[i] + carry;
                    U[i + j] = static_cast<uint64_t>(sum);
                    carry = sum >> 64;
                }
                U[j + n] += (uint64_t)carry;
            }
        }
        UintN r;
        uint64_t carry = 0;
        for (int i = m - 1; i >= 0; i--) {
            uint64_t current = U[i];
            r.d[i] = (current >> shift) | (carry << (64 - shift));
            carry = current & ((1ULL << shift) - 1);
        }
        return {q, r};
    }

    UintN operator/(const UintN& rhs) const { return this->divrem(rhs).first; }
    UintN& operator/=(const UintN& rhs) { return *this = *this / rhs; }

    UintN operator%(const UintN& rhs) const { return this->divrem(rhs).second; }
    UintN& operator%=(const UintN& rhs) { return *this = *this % rhs; }

    bool operator==(const UintN& rhs) const { return d == rhs.d; }
    bool operator!=(const UintN& rhs) const { return d != rhs.d; }
    std::strong_ordering operator<=>(const UintN& rhs) const {
        for (int i = N - 1; i >= 0; i--) {
            if (d[i] != rhs.d[i]) return d[i] <=> rhs.d[i];
        }
        return std::strong_ordering::equal;
    }

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
};

template <int N> struct IntN {
  private:
    UintN<N> d;

  public:
    const std::array<uint64_t, N>& data() const { return d.data(); }

    IntN() = default;
    IntN(UintN<N> d) : d(d) {}

    template <typename T>
        requires std::is_integral_v<T>
    IntN(T v) : d(v) {}

    explicit IntN(const std::string& s) : d(s) {}
    operator UintN<N>() const { return d; }

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

    bool is_negative() const { return data()[N - 1] >> 63; }

    IntN abs() const { return is_negative() ? -*this : *this; }
};

}  // namespace yosupo
