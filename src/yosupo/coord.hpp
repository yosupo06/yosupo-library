#pragma once

#include <array>
#include <cassert>
#include <ranges>
#include <string>
#include <utility>

namespace yosupo {

struct Coord {
  public:
    constexpr Coord() : d{0, 0} {}
    constexpr Coord(int _r, int _c) : d{_r, _c} {}
    constexpr Coord(const std::pair<int, int>& p) : d{p.first, p.second} {}

    bool operator==(const Coord& other) const { return d == other.d; }

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

    int& r() { return d[0]; }
    int& c() { return d[1]; }
    const int& r() const { return d[0]; }
    const int& c() const { return d[1]; }

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

    Coord move4(int dir) const {
        static constexpr std::array<Coord, 4> d4 = {Coord(0, 1), Coord(1, 0),
                                                    Coord(0, -1), Coord(-1, 0)};
        return *this + d4[dir];
    }

    auto move4() const {
        return std::views::iota(0, 4) | std::views::transform([this](int dir) {
                   return this->move4(dir);
               });
    }

    Coord move8(int dir) const {
        static constexpr std::array<Coord, 8> d8 = {
            Coord(0, 1),  Coord(1, 1),   Coord(1, 0),  Coord(1, -1),
            Coord(0, -1), Coord(-1, -1), Coord(-1, 0), Coord(-1, 1)};
        assert(0 <= dir && dir < 8);
        return *this + d8[dir];
    }

    auto move8() const {
        return std::views::iota(0, 8) | std::views::transform([this](int dir) {
                   return this->move8(dir);
               });
    }

    bool contain(const Coord& t) const {
        return 0 <= t.r() && t.r() < r() && 0 <= t.c() && t.c() < c();
    }

    auto cells() const {
        return std::views::iota(0, r()) | std::views::transform([this](int i) {
                   return std::views::iota(0, c()) |
                          std::views::transform(
                              [i](int j) { return Coord(i, j); });
               }) |
               std::views::join;
    }

  private:
    std::array<int, 2> d;
};

}  // namespace yosupo
