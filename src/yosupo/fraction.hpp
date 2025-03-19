#pragma once

#include <compare>
#include <string>

#include "yosupo/dump.hpp"
#include "yosupo/math.hpp"

namespace yosupo {

template <class T> struct Fraction {
    Fraction() : a(T(0)), b(T(1)) {}
    Fraction(T x) : a(x), b(T(1)) {}

    Fraction(T _a, T _b) {
        T g = yosupo::gcd(_a, _b);
        if (yosupo::sgn(_b) < 0) g = -g;
        a = _a / g;
        b = _b / g;
    }
    T numer() const { return a; }
    T denom() const { return b; }

    Fraction operator-() const { return {-a, b}; }
    Fraction operator+(const Fraction& r) const {
        return {r.b * a + b * r.a, b * r.b};
    }
    Fraction operator-(const Fraction& r) const { return *this + (-r); }
    Fraction operator*(const Fraction& r) const { return {a * r.a, b * r.b}; }
    Fraction operator/(const Fraction& r) const { return {a * r.b, b * r.a}; }
    Fraction& operator+=(const Fraction& r) { return *this = *this + r; }
    Fraction& operator-=(const Fraction& r) { return *this = *this - r; }
    Fraction& operator*=(const Fraction& r) { return *this = *this * r; }
    Fraction& operator/=(const Fraction& r) { return *this = *this / r; }

    bool operator==(const Fraction& r) const { return a == r.a && b == r.b; }
    bool operator!=(const Fraction& r) const { return !(*this == r); }
    std::strong_ordering operator<=>(const Fraction& rhs) const {
        return a * rhs.b <=> b * rhs.a;
    }

    int sgn() const { return yosupo::sgn(a); }
    Fraction abs() const { return (yosupo::sgn(a) < 0) ? -*this : *this; }

    std::string dump() const {
        if (b == T(1)) return yosupo::dump(a);
        return yosupo::dump(a) + "/" + yosupo::dump(b);
    }

  private:
    T a, b;
};
}  // namespace yosupo
