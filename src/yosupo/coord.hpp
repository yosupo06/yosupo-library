#pragma once

#include <cassert>
#include <iostream>
#include <utility>

namespace yosupo {

struct Coord {
    int r, c;

    Coord() : r(0), c(0) {}
    Coord(int r, int c) : r(r), c(c) {}
    Coord(const std::pair<int, int>& p) : r(p.first), c(p.second) {}

    Coord& operator+=(const Coord& other) {
        r += other.r;
        c += other.c;
        return *this;
    }

    Coord operator+(const Coord& other) const {
        Coord result = *this;
        result += other;
        return result;
    }

    Coord& operator-=(const Coord& other) {
        r -= other.r;
        c -= other.c;
        return *this;
    }

    Coord operator-(const Coord& other) const {
        Coord result = *this;
        result -= other;
        return result;
    }

    int& operator[](int index) {
        if (index == 0) return r;
        if (index == 1) return c;
        assert(false);
    }

    const int& operator[](int index) const {
        if (index == 0) return r;
        if (index == 1) return c;
        assert(false);
    }

    operator std::pair<int, int>() const { return std::make_pair(r, c); }

    friend std::ostream& operator<<(std::ostream& os, const Coord& coord) {
        os << "(" << coord.r << ", " << coord.c << ")";
        return os;
    }
};

}  // namespace yosupo
