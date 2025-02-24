#pragma once

#include <compare>
#include <iostream>

namespace yosupo {

template <class T> struct Point {
    T x, y;
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

    bool operator==(const Point& r) const { return x == r.x && y == r.y; }
    bool operator!=(const Point& r) const { return !(*this == r); }
    std::strong_ordering operator<=>(const Point& r) const {
        auto cmp = x <=> r.x;
        return cmp == 0 ? y <=> r.y : cmp;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << "P(" << p.x << ", " << p.y << ")";
    }
};

template <class T> T crs(Point<T> a, Point<T> b) {
    return a.x * b.y - a.y * b.x;
}
template <class T> T dot(Point<T> a, Point<T> b) {
    return a.x * b.x + a.y * b.y;
}

}  // namespace yosupo
