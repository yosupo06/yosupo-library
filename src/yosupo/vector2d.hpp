#pragma once

#include <cassert>
#include <vector>

#include "yosupo/coord.hpp"
#include "yosupo/dump.hpp"

namespace yosupo {

template <class T> struct Vector2D {
    int h, w;
    std::vector<T> d;

    Vector2D() : h(0), w(0), d() {}
    Vector2D(int _h, int _w) : h(_h), w(_w), d(h * w) {}

    T& operator[](const Coord& idx) { return d[idx.r * w + idx.c]; }
    const T& operator[](const Coord& idx) const { return d[idx.r * w + idx.c]; }

    static Vector2D e(int n) {
        Vector2D mat(n, n);
        for (int i = 0; i < n; i++) {
            mat[{i, i}] = T(1);
        }
        return mat;
    }

    Vector2D& operator+=(const Vector2D& rhs) {
        assert(h == rhs.h);
        assert(w == rhs.w);
        for (int i = 0; i < h * w; i++) {
            d[i] += rhs.d[i];
        }
        return *this;
    }
    Vector2D& operator-=(const Vector2D& rhs) {
        assert(h == rhs.h);
        assert(w == rhs.w);
        for (int i = 0; i < h * w; i++) {
            d[i] -= rhs.d[i];
        }
        return *this;
    }
    friend Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs) {
        return Vector2D(lhs) += rhs;
    }
    friend Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs) {
        return Vector2D(lhs) -= rhs;
    }

    friend Vector2D operator*(const Vector2D& lhs, const Vector2D& rhs) {
        assert(lhs.w == rhs.h);
        Vector2D res(lhs.h, rhs.w);
        Vector2D rhs_t = rhs.transpose();
        for (int i = 0; i < lhs.h; i++) {
            for (int j = 0; j < rhs.w; j++) {
                for (int k = 0; k < lhs.w; k++) {
                    res[{i, j}] += lhs[{i, k}] * rhs_t[{j, k}];
                }
            }
        }
        return res;
    }
    Vector2D& operator*=(const Vector2D& r) { return *this = *this * r; }

    Vector2D transpose() const {
        Vector2D res(w, h);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                res[{j, i}] = (*this)[{i, j}];
            }
        }
        return res;
    }

    Vector2D pow(long long n) const {
        assert(h == w);
        Vector2D x = *this, r = e(h);
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    std::string dump() const {
        std::string res;
        for (int i = 0; i < h; i++) {
            if (i) res += "\n";
            for (int j = 0; j < w; j++) {
                if (j) res += " ";
                res += yosupo::dump((*this)[{i, j}]);
            }
        }
        return res;
    }
};

}  // namespace yosupo
