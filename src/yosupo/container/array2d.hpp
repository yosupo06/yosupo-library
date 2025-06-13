#pragma once

#include <array>
#include <cassert>
#include <initializer_list>
#include <string>

#include "yosupo/coord.hpp"
#include "yosupo/dump.hpp"

namespace yosupo {

template <class T, int H, int W> struct Array2D {
    static_assert(H >= 0 && W >= 0);
    std::array<T, H * W> d;

    Array2D() : d{} {}
    Array2D(const T& val) { d.fill(val); }

    Array2D(std::initializer_list<std::initializer_list<T>> list) {
        assert(static_cast<int>(list.size()) == H);
        int i = 0;
        for (const auto& inner_list : list) {
            assert(static_cast<int>(inner_list.size()) == W);
            std::copy_n(inner_list.begin(), W, d.data() + i * W);
            i++;
        }
    }

    T& operator[](const Coord& idx) {
        assert(0 <= idx.r() && idx.r() < H);
        assert(0 <= idx.c() && idx.c() < W);
        return d[idx.r() * W + idx.c()];
    }
    const T& operator[](const Coord& idx) const {
        assert(0 <= idx.r() && idx.r() < H);
        assert(0 <= idx.c() && idx.c() < W);
        return d[idx.r() * W + idx.c()];
    }

    static Array2D e() {
        static_assert(H == W);
        Array2D mat;
        for (int i = 0; i < H; i++) {
            mat[{i, i}] = T(1);
        }
        return mat;
    }

    Array2D& operator+=(const Array2D& rhs) {
        for (int i = 0; i < H * W; i++) {
            d[i] += rhs.d[i];
        }
        return *this;
    }

    Array2D& operator-=(const Array2D& rhs) {
        for (int i = 0; i < H * W; i++) {
            d[i] -= rhs.d[i];
        }
        return *this;
    }

    friend Array2D operator+(const Array2D& lhs, const Array2D& rhs) {
        return Array2D(lhs) += rhs;
    }
    friend Array2D operator-(const Array2D& lhs, const Array2D& rhs) {
        return Array2D(lhs) -= rhs;
    }

    template <int K>
    friend Array2D<T, H, K> operator*(const Array2D& lhs,
                                      const Array2D<T, W, K>& rhs) {
        Array2D<T, H, K> res;
        Array2D<T, K, W> rhs_t = rhs.transpose();
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < K; j++) {
                for (int k = 0; k < W; k++) {
                    res[{i, j}] += lhs[{i, k}] * rhs_t[{j, k}];
                }
            }
        }
        return res;
    }

    Array2D& operator*=(const Array2D& r) {
        static_assert(H == W);
        return *this = *this * r;
    }

    Array2D<T, W, H> transpose() const {
        Array2D<T, W, H> res;
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                res[{j, i}] = (*this)[{i, j}];
            }
        }
        return res;
    }

    Array2D pow(long long n) const {
        static_assert(H == W);
        Array2D x = *this, r = e();
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    std::string dump() const {
        std::string res;
        for (int i = 0; i < H; i++) {
            if (i) res += "\n";
            for (int j = 0; j < W; j++) {
                if (j) res += " ";
                res += yosupo::dump((*this)[{i, j}]);
            }
        }
        return res;
    }

    constexpr int height() const { return H; }
    constexpr int width() const { return W; }
};

}  // namespace yosupo