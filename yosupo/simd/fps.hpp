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
    void set_freq(int n, mint x) {
        assert(0 <= n && n <= int(size()));
        v[n / 8].set(n % 8, x);
    }

    size_t size() const { return _size; }

    FPS pre(int n) const {
        int n2 = std::min(n, int(v.size() * 8));
        auto v2 = std::vector<mintx8>({v.begin(), v.begin() + (n2 + 7) / 8});
        return FPS(n2, v2).resize(n);
    }
    FPS& resize(int n) {
        _size = n;
        v.resize((n + 7) / 8);
        return clean_prefix();
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
        if (!size()) return *this;
        if (!rhs.size()) {
            *this = FPS();
            return *this;
        }
        _size = _size + int(rhs.size()) - 1;
        int nsize8 = (_size + 7) / 8;
        int z = 1 << atcoder::internal::ceil_pow2(nsize8);
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
        if (!lhs.size() || !rhs.size()) return FPS();
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

    FPS& operator<<=(int n) {
        if (n % 8) {
            // TODO: optimize
            auto v2 = to_vec();
            v2.insert(v2.begin(), n, mint());
            return *this = FPS(v2);
        }
        _size += n;
        v.insert(v.begin(), n / 8, mintx8());
        return *this;
    }
    friend FPS operator<<(const FPS& lhs, int n) {
        return FPS(lhs) <<= n;
    }

    FPS diff() const {
        // TODO: optimize
        if (size() == 0) return FPS();
        std::vector<mint> res = to_vec();
        for (int i = 1; i < int(size()); i++) res[i - 1] = res[i] * i;
        res.pop_back();
        return FPS(res);
    }

    FPS& diff_x_assign() {
        static std::vector<mintx8> is;
        while (is.size() < v.size()) {
            int m = int(is.size()) * 8;
            is.push_back(
                mintx8(m, m + 1, m + 2, m + 3, m + 4, m + 5, m + 6, m + 7));
        }
        for (int i = 0; i < int(v.size()); i++) {
            v[i] *= is[i];
        }
        return *this;
    }
    // diff_x() = diff() * x
    FPS diff_x() const {
        return FPS(*this).diff_x_assign();
    }

    FPS inte() const {
        // TODO: optimize
        std::vector<mint> res = to_vec();
        res.push_back(mint(0));
        for (int i = int(size()); i >= 1; i--) res[i] = res[i - 1] * yosupo::inv<mint>(i);
        res[0] = mint(0);
        return FPS(res);
    }

    FPS& ix_inte_assign() {
        static std::vector<mintx8> is;
        while (is.size() < v.size()) {
            int m = int(is.size()) * 8;
            std::array<mint, 8> x;
            for (int i = 0; i < 8; i++) {
                x[i] = (m + i == 0 ? 0 : yosupo::inv<mint>(m + i));
            }
            is.push_back(x);
        }
        for (int i = 0; i < int(v.size()); i++) {
            v[i] *= is[i];
        }
        return *this;
    }
    // ix_inte() = inte(this / x)
    FPS ix_inte() const {
        return FPS(*this).ix_inte_assign();
    }

    FPS& ntt() {
        assert((size() & -size()) == size()); // size() = 2^n
        butterfly(v);
        return clean_prefix();
    }
    FPS& intt() {
        assert((size() & -size()) == size());  // size() = 2^n
        butterfly_inv(v);
        mintx8 iz = mint(size()).inv();
        for (auto& x : v) {
            x *= iz;
        }
        return clean_prefix();
    }

    FPS& dot_assign(const FPS& rhs) {
        for (int i = 0; i < int(std::min(v.size(), rhs.v.size())); i++) {
            v[i] *= rhs.v[i];
        }
        return *this;
    }

    FPS& clean_range(int s, int t) {
        assert(0 <= s && s <= t && t <= int(size()));
        {
            // TODO: optimize
            while (s < t && s % 8) {
                set_freq(s, 0);
                s++;
            }
            while (s < t && t % 8) {
                t--;
                set_freq(t, 0);
            }
        }
        for (int i = s / 8; i < t / 8; i++) {
            v[i] = mintx8();
        }
        return *this;
    }

    // f_ntt = f.pre(2 * d).ntt()
    FPS& inv_extend(FPS& f, const FPS& f_ntt) const {
        assert((f.size() & -f.size()) == f.size());
        int d = int(f.size());
        FPS buf1 = pre(2 * d);
        buf1.ntt();
        buf1.dot_assign(f_ntt);
        buf1.intt();
        buf1.clean_range(0, d);
        buf1.ntt();
        buf1.dot_assign(f_ntt);
        buf1.intt();
        return f.resize(2 * d) -= buf1.clean_range(0, d);
    }
    // input:
    //  f.size() = 2^n
    //  f = inv(f.size())
    // output:
    //  f <- inv(f.size() * 2)
    FPS& inv_extend(FPS& f) const {
        assert((f.size() & -f.size()) == f.size());
        int d = int(f.size());
        return inv_extend(f, f.pre(2 * d).ntt());
    }

    FPS inv(int n) const {        
        assert(size() >= 1);
        FPS res({freq(0).inv()});
        for (int d = 1; d < n; d *= 2) {
            inv_extend(res);
        }
        return res.resize(n);
    }

    FPS exp(int n) const {
        assert(freq(0) == 0);
        FPS f({1}), g({1});
        for (int d = 1; d < n; d *= 2) {
            // t = (*this)
            // f = t.exp(d)
            // g = t.exp(d).inv(d)

            FPS f2 = f.pre(2 * d).ntt();
            FPS g2 = g.pre(2 * d).ntt();

            FPS dt = pre(d).diff_x_assign();

            // w = (f * dt) % (x^m - 1)
            // w % (x^m) = f.diff_x() (because f.diff_x() = f * t.pre(d).diff_x())
            FPS w = dt;
            w.ntt();
            w.dot_assign(f2);
            w.intt();
            // w = (f * dt - f.diff_x()) / x^d
            w -= f.diff_x();

            // w = g * (f * dt - f.diff_x())
            w.resize(2 * d);
            w.ntt();
            w.dot_assign(g2);
            w.intt();
            w.resize(d);
            w <<= d;

            // w = g * (f * dt - f.diff_x()) - dt
            w -= dt;

            FPS z = pre(2 * d);
            z += w.ix_inte_assign();
            z.ntt();
            z.dot_assign(f2);
            z.intt();
            f.resize(2 * d) += z.clean_range(0, d);

            if (d * 2 < n) {
                f.inv_extend(g, g2);
            }
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
        clean_prefix();
    }

    size_t size8() const { return (_size + 7) / 8; }

    FPS& clean_prefix() {
        if (_size % 8) {
            v.back().clear((unsigned char)(-1U << (_size % 8)));
        }
        return *this;
    }
};

}  // namespace yosupo
