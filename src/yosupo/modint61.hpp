#pragma once

#include <cassert>
#include <concepts>

namespace yosupo {

// modint of 2^61 - 1
struct ModInt61 {
    using mint = ModInt61;

  public:
    static constexpr long long mod() { return (1ULL << 61) - 1; }

    ModInt61() : x(0) {}

    template <class T>
        requires std::signed_integral<T> || std::same_as<T, __int128>
    constexpr ModInt61(T _x) {
        long long y = (long long)(_x % mod());
        if (y < 0) y += umod();
        x = (unsigned long long)(y);
    }
    template <class T>
        requires std::unsigned_integral<T> || std::same_as<T, unsigned __int128>
    constexpr ModInt61(T _x) {
        x = (unsigned long long)(_x % umod());
    }

    long long val() const { return x; }

    mint& operator++() {
        x++;
        if (x == umod()) x = 0;
        return *this;
    }
    mint& operator--() {
        if (x == 0) x = umod();
        x--;
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
        x += rhs.x;
        if (x >= umod()) x -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        x -= rhs.x;
        if (x >= umod()) x += umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        unsigned __int128 t = (unsigned __int128)(x)*rhs.x;
        x = (unsigned long long)((t >> 61) + (t & umod()));
        x = (x >= umod()) ? x - umod() : x;

        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint v = *this, r = 1;
        while (n) {
            if (n & 1) r *= v;
            v *= v;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        assert(x);
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
        return lhs.x == rhs.x;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return !(lhs == rhs);
    }

  private:
    unsigned long long x;
    static constexpr unsigned long long umod() { return mod(); }
};

}  // namespace yosupo
