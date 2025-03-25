#pragma once

#include <compare>
#include <string>

#include "yosupo/dump.hpp"
#include "yosupo/numeric.hpp"

namespace yosupo {

template <class T> struct Point {
    T x, y;
    Point() : x(T()), y(T()) {}
    Point(T _x, T _y) : x(_x), y(_y) {}

    Point operator-() const { return {-x, -y}; }
    Point operator+(const Point& r) const { return {x + r.x, y + r.y}; }
    Point operator-(const Point& r) const { return {x - r.x, y - r.y}; }
    Point operator*(const Point& r) const {
        return {x * r.x - y * r.y, x * r.y + y * r.x};
    }
    Point& operator+=(const Point& r) { return *this = *this + r; }
    Point& operator-=(const Point& r) { return *this = *this - r; }
    Point& operator*=(const Point& r) { return *this = *this * r; }

    friend Point operator*(const Point& l, const T& r) {
        return {l.x * r, l.y * r};
    }
    friend Point operator*(const T& l, const Point& r) {
        return {l * r.x, l * r.y};
    }
    Point& operator*=(const T& r) { return *this = *this * r; }

    friend Point operator/(const Point& l, const T& r) {
        return {l.x / r, l.y / r};
    }
    Point& operator/=(const T& r) { return *this = *this / r; }

    bool operator==(const Point& r) const { return x == r.x && y == r.y; }
    bool operator!=(const Point& r) const { return !(*this == r); }
    std::strong_ordering operator<=>(const Point& r) const {
        auto cmp = x <=> r.x;
        return cmp == 0 ? y <=> r.y : cmp;
    }

    std::string dump() const {
        return "(" + yosupo::dump(x) + ", " + yosupo::dump(y) + ")";
    }
};

template <class T> T crs(Point<T> a, Point<T> b) {
    return a.x * b.y - a.y * b.x;
}
template <class T> T dot(Point<T> a, Point<T> b) {
    return a.x * b.x + a.y * b.y;
}

// -2, -1, 0, 1, 2 : front, clock, on, cclock, back
template <class T> int ccw(const Point<T>& src, const Point<T>& trg) {
    int s = sgn(crs(src, trg));
    if (s) return s;

    if (trg == Point<T>() || src == trg) return 0;
    if (sgn(dot(src, trg)) < 0) return 2;
    if (sgn(dot(-src, trg - src)) < 0) return -2;
    return 0;
}
template <class T>
int ccw(const Point<T>& orig, const Point<T>& src, const Point<T>& trg) {
    return ccw(src - orig, trg - orig);
}

/*
    arg(l) <=> arg(r)を返す
    - arg(p)は(-PI, PI]の範囲に収まる
    - arg(P(-1, 0)) = PI,
    - arg(P(0, 0)) = 0
    - arg(P(1, 0)) = 0
*/
template <class T> std::strong_ordering cmp_arg(Point<T> l, Point<T> r) {
    auto psgn = [&](Point<T> p) {
        if (int u = sgn(p.y)) return u;
        if (sgn(p.x) == -1) return 2;
        return 0;
    };
    int lsgn = psgn(l), rsgn = psgn(r);
    if (lsgn < rsgn) return std::strong_ordering::less;
    if (lsgn > rsgn) return std::strong_ordering::greater;
    return sgn(crs(r, l)) <=> 0;
}

}  // namespace yosupo
