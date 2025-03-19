#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "atcoder/convolution.hpp"
#include "yosupo/comb.hpp"

namespace yosupo {

template <class T> struct FPS {
  private:
    std::vector<T> v;
    FPS& shrink() {
        while (v.size() && v.back() == T(0)) v.pop_back();
        return *this;
    }

  public:
    FPS() {}
    FPS(const std::vector<T>& _v) : v(_v) { shrink(); }

    int size() const { return int(v.size()); }
    void set(int p, T x) {
        if (int(v.size()) <= p && x == T(0)) return;
        if (int(v.size()) <= p) v.resize(p + 1);
        v[p] = x;
    }
    T freq(int p) const { return (p < size()) ? v[p] : T(0); }

    FPS& operator+=(const FPS& rhs) {
        if (size() < rhs.size()) v.resize(rhs.size());
        for (int i = 0; i < rhs.size(); i++) {
            v[i] += rhs.v[i];
        }
        return shrink();
    }
    FPS& operator-=(const FPS& rhs) {
        if (size() < rhs.size()) v.resize(rhs.size());
        for (int i = 0; i < rhs.size(); i++) {
            v[i] -= rhs.v[i];
        }
        return shrink();
    }
    FPS& operator*=(const FPS& rhs) {
        v = atcoder::convolution(v, rhs.v);
        return shrink();
    }
    FPS& operator*=(const T& r) {
        for (int i = 0; i < size(); i++) {
            v[i] *= r;
        }
        return shrink();
    }
    FPS& operator<<=(int s) {
        v.insert(v.begin(), s, T(0));
        return shrink();
    }
    FPS& operator>>=(int s) {
        if (size() <= s) return FPS();
        v.erase(v.begin(), v.begin() + s);
        return shrink();
    }

    friend FPS operator+(const FPS& lhs, const FPS& rhs) {
        return FPS(lhs) += rhs;
    }
    friend FPS operator-(const FPS& lhs, const FPS& rhs) {
        return FPS(lhs) -= rhs;
    }
    friend FPS operator*(const FPS& lhs, const FPS& rhs) {
        return FPS(lhs) *= rhs;
    }
    friend FPS operator<<(const FPS& lhs, const int s) {
        return FPS(lhs) <<= s;
    }
    friend FPS operator>>(const FPS& lhs, const int s) {
        return FPS(lhs) >>= s;
    }
    friend bool operator==(const FPS& lhs, const FPS& rhs) {
        return lhs.v == rhs.v;
    }

    FPS integral() const {
        std::vector<T> res(size() + 1);
        for (int i = 0; i < size(); i++) res[i + 1] = freq(i) / (i + 1);
        return res;
    }
    T eval(T x) const {
        T sum = 0, base = 1;
        for (int i = 0; i < size(); i++) {
            sum += base * v[i];
            base *= x;
        }
        return sum;
    }
    FPS taylor_shift(T c) const {
        std::vector<T> q(size());
        for (int i = 0; i < size(); i++) {
            T base = 1;
            for (int j = 0; j <= i; j++) {
                q[i - j] += v[i] * base * comb<T>(i, j);
                base *= c;
            }
        }
        return q;
    }

    friend std::ostream& operator<<(std::ostream& os, const FPS& p) {
        if (p.size() == 0) return os << "0";
        for (auto i = 0; i < p.size(); i++) {
            if (p.v[i].val()) {
                os << p.v[i] << "x^" << i;
                if (i != p.size() - 1) os << "+";
            }
        }
        return os;
    }

    // (f^n).trim(m)
    FPS pow(int n, int m = -1) const {
        assert(n > 0);
        T p0 = freq(0);
        assert(p0 != T(0));
        if (m == -1) m = n * (size() - 1) + 1;
        T ip0 = p0.inv();
        std::vector<T> val(m);
        val[0] = p0.pow(n);
        for (int i = 0; i < m - 1; i++) {
            T sum = 0;
            for (int j = 1; j <= std::min(i + 1, size() - 1); j++) {
                sum += T(j * n - i + j - 1) * val[i + 1 - j] * v[j];
            }
            val[i + 1] = sum * ip0 * inv<T>(i + 1);
        }
        return val;
    }
};

template <class T> FPS<T> berlekamp_massey(const std::vector<T>& s) {
    int n = int(s.size());
    std::vector<T> b = {T(-1)}, c = {T(-1)};
    T y = T(1);
    for (int ed = 1; ed <= n; ed++) {
        int l = int(c.size()), m = int(b.size());
        T x = 0;
        for (int i = 0; i < l; i++) {
            x += c[i] * s[ed - l + i];
        }
        b.push_back(0);
        m++;
        if (x == T(0)) continue;
        T freq = x / y;
        if (l < m) {
            // use b
            auto tmp = c;
            c.insert(begin(c), m - l, T(0));
            for (int i = 0; i < m; i++) {
                c[m - 1 - i] -= freq * b[m - 1 - i];
            }
            b = tmp;
            y = x;
        } else {
            // use c
            for (int i = 0; i < m; i++) {
                c[l - 1 - i] -= freq * b[m - 1 - i];
            }
        }
    }
    return c;
}

}  // namespace yosupo
