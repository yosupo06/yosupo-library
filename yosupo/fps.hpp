#pragma once

#include "yosupo/modint.hpp"

namespace yosupo {

template <class T> struct FPS {
  private:
    std::vector<T> v;
    ModPoly& shrink() {
        while (v.size() && v.back() == T(0)) v.pop_back();
        return *this;
    }

  public:
    ModPoly() {}
    ModPoly(const std::vector<T>& _v) : v(_v) { shrink(); }

    int size() const { return int(v.size()); }
    void set(int p, T x) {
        if (int(v.size()) <= p && x == T(0)) return;
        while (int(v.size()) < p) v.resize(p + 1);
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
            v[i] -= rhs[i];
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
        if (size() <= s) return ModPoly();
        v.erase(v.begin(), v.begin() + s);
        return shrink();
    }

    friend FPS operator+(const FPS& lhs, const FPS& rhs) {
        return ModPoly(lhs) += rhs;
    }
    friend FPS operator-(const FPS& lhs, const FPS& rhs) {
        return ModPoly(lhs) -= rhs;
    }
    friend FPS operator*(const FPS& lhs, const FPS& rhs) {
        return ModPoly(lhs) *= rhs;
    }
    friend FPS operator<<(const FPS& lhs, const int s) {
        return ModPoly(lhs) <<= s;
    }
    friend FPS operator>>(const FPS& lhs, const int s) {
        return ModPoly(lhs) >>= s;
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
};

}
