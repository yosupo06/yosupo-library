#pragma once

#include <cstddef>
#include <string>
#include <vector>

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
    ModVec(const std::vector<modint>& _v) : v(_v) {}

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

    size_t size() const { return v.size(); }

    std::string dump() const { return ::yosupo::dump(v); }

  private:
    std::vector<modint> v;
};

}  // namespace yosupo
