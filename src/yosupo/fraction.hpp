#pragma once

#include <iostream>

#include "yosupo/math.hpp"

namespace yosupo {

template <class T>
concept Printable = requires(std::ostream& os, const T& t) { os << t; };

template <class T> struct Fraction {
    Fraction() : a(T(0)), b(T(1)) {}
    Fraction(T x) : a(x), b(T(1)) {}

    Fraction(T _a, T _b) {
        T g = yosupo::gcd(_a, _b);
        if (_b < 0) g = -g;
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

    Fraction abs() const { return (a < 0) ? -*this : *this; }

    friend std::ostream& operator<<(std::ostream& os, Fraction x)
        requires Printable<T>
    {
        return os << x.a << "/" << x.b;
    }

  private:
    T a, b;
};
}  // namespace yosupo
