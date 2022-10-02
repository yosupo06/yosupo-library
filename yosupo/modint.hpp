#pragma once

#include "atcoder/modint.hpp"

#include <iostream>

namespace atcoder {

template <int MOD>
std::ostream& operator<<(std::ostream& os, const static_modint<MOD>& x) {
    return os << x.val();
}

template <int ID>
std::ostream& operator<<(std::ostream& os, const dynamic_modint<ID>& x) {
    return os << x.val();
}

}  // namespace atcoder

namespace yosupo {

template <int MOD> using static_modint = atcoder::static_modint<MOD>;

template <int ID> using dynamic_modint = atcoder::dynamic_modint<ID>;

using modint998244353 = atcoder::modint998244353;
using modint1000000007 = atcoder::modint1000000007;
using modint = atcoder::modint;

// modint of 2^61 - 1
struct modint61 {
    using mint = modint61;

  public:
    static constexpr long long mod() { return (1ULL << 61) - 1; }
    static mint raw(long long v) {
        mint x;
        x._v = v;
        return x;
    }

    modint61() : _v(0) {}

    template <class T, atcoder::internal::is_signed_int_t<T>* = nullptr>
    modint61(T v) {
        long long x = (long long)(v % (long long)(umod()));
        if (x < 0) x += umod();
        _v = (unsigned long long)(x);
    }
    template <class T, atcoder::internal::is_unsigned_int_t<T>* = nullptr>
    modint61(T v) {
        _v = (unsigned long long)(v % umod());
    }

    unsigned long long val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        __uint128_t z = _v;
        z *= rhs._v;
        _v = (unsigned long long)(z % umod());
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        assert(_v);
        return pow(umod() - 2);
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned long long _v;
    static constexpr unsigned long long umod() { return mod(); }
};

std::ostream& operator<<(std::ostream& os, const modint61& x) {
    return os << x.val();
}

namespace internal {

template <class H> auto update(const H& h, const modint61& x) {
    return update(h, x.val());
}

}  // namespace internal

}  // namespace yosupo
