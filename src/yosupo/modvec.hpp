#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "yosupo/convolution.hpp"
#include "yosupo/dump.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/modint8.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <i32 MOD> struct ModVec {
  private:
    using modint = ModInt<MOD>;
    using modint8 = ModInt8<MOD>;

  public:
    ModVec() {}
    explicit ModVec(size_t n) : v(n) {}
    ModVec(const std::vector<modint>& _v) : v(_v) {}
    ModVec(std::initializer_list<modint> init) : v(init) {}

    modint& operator[](int p) { return v[p]; }
    const modint& operator[](int p) const { return v[p]; }

    bool operator==(const ModVec& rhs) const { return v == rhs.v; }

    ModVec& operator+=(const ModVec& rhs) {
        if (size() < rhs.size()) v.resize(rhs.size());
        // TODO: simd
        for (int i = 0; i < std::ssize(rhs); i++) {
            v[i] += rhs.v[i];
        }
        return *this;
    }
    friend ModVec operator+(const ModVec& lhs, const ModVec& rhs) {
        return ModVec(lhs) += rhs;
    }
    ModVec& operator-=(const ModVec& rhs) {
        if (size() < rhs.size()) v.resize(rhs.size());
        // TODO: simd
        for (int i = 0; i < std::ssize(rhs); i++) {
            v[i] -= rhs.v[i];
        }
        return *this;
    }
    friend ModVec operator-(const ModVec& lhs, const ModVec& rhs) {
        return ModVec(lhs) -= rhs;
    }

    friend ModVec operator*(const ModVec& lhs, const ModVec& rhs) {
        return ModVec(convolution(lhs.v, rhs.v));
    }
    ModVec& operator*=(const ModVec& rhs) { return *this = *this * rhs; }

    size_t size() const { return v.size(); }

    std::string dump() const { return ::yosupo::dump(v); }

    static ModVec prod(std::vector<ModVec> pols) {
        if (pols.empty()) return ModVec({modint(1)});
        if (pols.size() == 1) return pols[0];

        // sort by the first
        std::priority_queue<std::pair<int, int>,
                            std::vector<std::pair<int, int>>, std::greater<>>
            que;
        for (int i = 0; i < std::ssize(pols); i++) {
            que.emplace(pols[i].size(), i);
        }

        while (que.size() > 1) {
            auto [a, i] = que.top();
            que.pop();
            auto [b, j] = que.top();
            que.pop();
            pols[i] *= pols[j];
            pols[j] = ModVec();

            que.emplace(pols[i].size(), i);
        }

        auto [_, i] = que.top();
        return pols[i];
    }

  private:
    std::vector<modint> v;
};

using ModVec998244353 = ModVec<998244353>;
using ModVec1000000007 = ModVec<1000000007>;

}  // namespace yosupo
