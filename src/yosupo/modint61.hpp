#pragma once

#include <cassert>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>

#include "yosupo/random.hpp"

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
    friend std::ostream& operator<<(std::ostream& os, const mint& m) {
        return os << "(" << m.val() << ")";
    }

  private:
    unsigned long long x;
    static constexpr unsigned long long umod() { return mod(); }
};

namespace hash {

inline ModInt61 base = yosupo::uniform(0LL, ModInt61::mod() - 1);
inline ModInt61 ibase = base.inv();

inline std::vector<ModInt61> pow_base = {1}, pow_ibase = {1};

inline ModInt61 pw_b(int k) {
    while (std::ssize(pow_base) <= k) {
        pow_base.push_back(pow_base.back() * base);
    }
    return pow_base[k];
}

inline ModInt61 pw_ib(int k) {
    while (std::ssize(pow_ibase) <= k) {
        pow_ibase.push_back(pow_ibase.back() * ibase);
    }
    return pow_ibase[k];
}

}  // namespace hash

struct RHash {
    RHash() : sz(0), h(0) {}
    RHash(ModInt61 _h) : sz(1), h(_h) {}
    RHash(int _sz, ModInt61 _h) : sz(_sz), h(_h) {}
    RHash(const std::string& s) : sz(int(s.size())), h(0) {
        for (int i = 0; i < sz; i++) {
            h += s[i] * hash::pw_b(i);
        }
    }

    // H(l) + H(r) = H(lr)
    friend RHash operator+(const RHash& lhs, const RHash& rhs) {
        return RHash(lhs.sz + rhs.sz, lhs.h + rhs.h * hash::pw_b(lhs.sz));
    }
    RHash& operator+=(const RHash& r) { return *this = *this + r; }

    friend bool operator==(const RHash& lhs, const RHash& rhs) {
        return lhs.h == rhs.h;
    }
    friend bool operator!=(const RHash& lhs, const RHash& rhs) {
        return !(lhs.h == rhs.h);
    }

    // H(lr).strip_left(H(l)) = H(r)
    RHash strip_left(const RHash& l) const {
        return RHash(sz - l.sz, (h - l.h) * hash::pw_ib(l.sz));
    }
    // H(lr).strip_right(H(r)) = H(l)
    RHash strip_right(const RHash& r) const {
        return RHash(sz - r.sz, h - r.h * hash::pw_b(r.sz));
    }

    friend std::ostream& operator<<(std::ostream& os, const RHash& x) {
        return os << "(" << x.sz << ", " << x.h << ")";
    }

  private:
    int sz;
    ModInt61 h;
};

}  // namespace yosupo
