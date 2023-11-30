#pragma once

#include "atcoder/internal_bit"
#include "yosupo/modint.hpp"
#include "yosupo/comb.hpp"
#include "yosupo/simd/convolution.hpp"
#include "yosupo/simd/modint.hpp"

#include <algorithm>
#include <vector>

namespace yosupo {

template <int MOD> struct FPS {
    using mint = static_modint<MOD>;
    using mintx8 = modintx8<MOD>;

  public:
    FPS() : _size(0) {}
    FPS(const std::vector<mint>& _v) : _size(int(_v.size())) {
        int size8 = (_size + 7) / 8;
        v.resize(size8);
        for (int i = 0; i < size8; i++) {
            std::array<mint, 8> buf;
            for (int j = 0; j < 8 && (i * 8 + j) < _size; j++) {
                buf[j] = _v[i * 8 + j];
            }
            v[i] = buf;
        }
    }

    mint freq(int n) const { return v[n / 8].at(n % 8); }

    size_t size() const { return _size; }

    FPS pre(int n) const {
        n = std::min(n, int(v.size() * 8));
        auto v2 = std::vector<mintx8>({v.begin(), v.begin() + (n + 7) / 8});
        if (n % 8) {
            v2.back().clear((unsigned char)(-1U << (n % 8)));
        }
        return FPS(n, v2);
    }

    FPS& operator+=(const FPS& rhs) {
        _size = std::max(_size, int(rhs.size()));

        int n = int(rhs.v.size());
        if (int(v.size()) < n) v.resize(n);
        for (int i = 0; i < n; i++) {
            v[i] += rhs.v[i];
        }
        return *this;
    }
    friend FPS operator+(const FPS& lhs, const FPS& rhs) {
        return FPS(lhs) += rhs;
    }
    FPS& operator-=(const FPS& rhs) {
        _size = std::max(_size, int(rhs.size()));

        int n = int(rhs.v.size());
        if (int(v.size()) < n) v.resize(n);
        for (int i = 0; i < n; i++) {
            v[i] -= rhs.v[i];
        }
        return *this;
    }
    friend FPS operator-(const FPS& lhs, const FPS& rhs) {
        return FPS(lhs) -= rhs;
    }

    FPS& operator*=(const FPS& rhs) {
        _size = _size + int(rhs.size()) - 1;
        int nsize8 = (_size + 7) / 8;
        int z = (int)atcoder::internal::bit_ceil((unsigned int)nsize8);
        auto rv = rhs.v;
        v.resize(z);
        rv.resize(z);
        butterfly(v);
        butterfly(rv);
        for (int i = 0; i < z; i++) {
            v[i] *= rv[i];
        }
        butterfly_inv(v);
        v.resize(nsize8);
        modintx8<MOD> iz = static_modint<MOD>(8 * z).inv();
        for (int i = 0; i < nsize8; i++) {
            v[i] *= iz;
        }
        return *this;
    }
    friend FPS operator*(const FPS& lhs, const FPS& rhs) {
        return FPS(lhs) *= rhs;
    }

    FPS& operator*=(const mint& rhs) {
        mintx8 y = rhs;
        for (auto& x : v) {
            x *= y;
        }
        return *this;
    }
    friend FPS operator*(const FPS& lhs, const mint& rhs) {
        return FPS(lhs) *= rhs;
    }

    FPS diff() const {
        if (size() == 0) return FPS();
        std::vector<mint> res = to_vec();
        for (int i = 1; i < int(size()); i++) res[i - 1] = res[i] * i;
        res.pop_back();
        return FPS(res);
    }

    FPS inte() const {
        std::vector<mint> res = to_vec();
        res.push_back(mint(0));
        for (int i = int(size()); i >= 1; i--) res[i] = res[i - 1] * yosupo::inv<mint>(i);
        res[0] = mint(0);
        return FPS(res);
    }

    FPS inv(int n) const {        
        assert(size() >= 1);
        auto naive_conv = [&](mintx8 l, mintx8 r) {
            auto lv = l.to_array();
            auto rv = r.to_array();
            std::array<mint, 8> z;
            for (int i = 0; i < 8; i++) {
                for (int j = 0; i + j < 8; j++) {
                    z[i + j] += lv[i] * rv[j];
                }
            }
            return mintx8(z);
        };
        
        mint if0 = freq(0).inv();

        mintx8 one;
        one.set(0, 1);
        mintx8 x = one - v[0] * if0;
        mintx8 x2 = naive_conv(x, x);
        mintx8 d0 = naive_conv(naive_conv(one + x, one + x2), one + naive_conv(x2, x2));

        std::vector<mintx8> res = {d0 * if0};

        for (int d = 8; d < n; d *= 2) {
            // res <- (2 * res - res * res * pre(2 * d)).pre(2 * d)
            mint i2 = mint(2 * d).inv();
            std::vector<mintx8> buf1(2 * d / 8);
            copy_n(v.begin(), std::min(int(v.size()), 2 * d / 8), buf1.begin());

            std::vector<mintx8> buf2 = res;
            buf2.resize(2 * d / 8);

            butterfly(buf1);
            butterfly(buf2);
            for (int i = 0; i < 2 * d / 8; i++) {
                buf1[i] *= buf2[i];
            }
            butterfly_inv(buf1);
            for (int i = 0; i < 2 * d / 8; i++) {
                buf1[i] *= i2;
            }
            for (int i = 0; i < d / 8; i++) {
                buf1[i] = mintx8();
            }
            butterfly(buf1);
            for (int i = 0; i < 2 * d / 8; i++) {
                buf1[i] *= buf2[i];
            }
            butterfly_inv(buf1);
            for (int i = 0; i < 2 * d / 8; i++) {
                buf1[i] *= i2;
            }

            res.resize(2 * d / 8);
            for (int i = d / 8; i < 2 * d / 8; i++) {
                res[i] = -buf1[i];
            }
        }
        return FPS(int(res.size() * 8), res).pre(n);
    }

    FPS exp(int n) const {
        assert(freq(0) == 0);
        FPS f({1}), g({1});
        for (int i = 1; i < n; i *= 2) {
            g = (g * mint(2) - f * g * g).pre(i);
            FPS q = diff().pre(i - 1);
            FPS w = (q + g * (f.diff() - f * q)).pre(2 * i - 1);
            f = (f + f * (*this - w.inte()).pre(2 * i)).pre(2 * i);
        }
        return f.pre(n);
    }
    FPS log(int n) const {
        assert(freq(0) == 1);
        auto f = pre(n);
        return (f.diff() * f.inv(n - 1)).pre(n - 1).inte();
    }

    std::vector<mint> to_vec() const {
        std::vector<mint> res(_size);
        for (int i = 0; i < (_size + 7) / 8; i++) {
            auto _v = v[i].to_array();
            for (int j = 0; j < 8 && (i * 8 + j) < _size; j++) {
                res[i * 8 + j] = _v[j];
            }
        }
        return res;
    }

  private:
    int _size;
    std::vector<mintx8> v;

    FPS(const int n, const std::vector<mintx8>& _v) : _size(n), v(_v) {
        assert((n + 7) / 8 == int(v.size()));
    }

    size_t size8() const { return (_size + 7) / 8; }
};

}  // namespace yosupo
