#pragma once

#include <cassert>
#include <iostream>
#include <utility>

namespace yosupo {

struct Coord {
    int d[2];

    Coord() : d{0, 0} {}
    Coord(int _r, int _c) : d{_r, _c} {}
    Coord(const std::pair<int, int>& p) : d{p.first, p.second} {}

    int& r() { return d[0]; }
    int& c() { return d[1]; }

    const int& r() const { return d[0]; }
    const int& c() const { return d[1]; }

    Coord& operator+=(const Coord& other) {
        d[0] += other.d[0];
        d[1] += other.d[1];
        return *this;
    }

    Coord operator+(const Coord& other) const {
        Coord result = *this;
        result += other;
        return result;
    }

    Coord& operator-=(const Coord& other) {
        d[0] -= other.d[0];
        d[1] -= other.d[1];
        return *this;
    }

    Coord operator-(const Coord& other) const {
        Coord result = *this;
        result -= other;
        return result;
    }

    int& operator[](int index) {
        if (index == 0) return d[0];
        if (index == 1) return d[1];
        assert(false);
    }

    const int& operator[](int index) const {
        if (index == 0) return d[0];
        if (index == 1) return d[1];
        assert(false);
    }

    operator std::pair<int, int>() const { return std::make_pair(d[0], d[1]); }

    std::string dump() const {
        return "(" + std::to_string(d[0]) + ", " + std::to_string(d[1]) + ")";
    }
};

}  // namespace yosupo
