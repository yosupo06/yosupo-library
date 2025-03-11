#pragma once

#include <vector>

#include "yosupo/modint.hpp"
#include "yosupo/modint8.hpp"

namespace yosupo {

template <i32 MOD> struct ModVec {
  private:
    using modint = ModInt<MOD>;
    using modint8 = ModInt8<MOD>;
    std::vector<modint> v;

  public:
    ModVec() {}
    ModVec(const std::vector<modint>& _v) : v(_v) {}

    modint& operator[](int p) { return v[p]; }
    const modint& operator[](int p) const { return v[p]; }

    bool operator==(const ModVec& rhs) const { return v == rhs.v; }
    bool operator!=(const ModVec& rhs) const { return v != rhs.v; }
};

}  // namespace yosupo
