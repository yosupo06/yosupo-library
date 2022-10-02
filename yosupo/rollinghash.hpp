#pragma once

#include "yosupo/comb.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/random.hpp"

namespace yosupo {

struct RollingHash {
  public:
    static void init(int n) {
        pow_base.resize(n);
        pow_ibase.resize(n);

        pow_base[0] = pow_ibase[0] = modint61(1);

        for (int i = 1; i < n; i++) {
          pow_base[i] = pow_base[i - 1] * base;
          pow_ibase[i] = pow_ibase[i - 1] * ibase;
        }
    }

    int len = 0;
    yosupo::modint61 h;
    RollingHash() : len(0), h(0) {}
    RollingHash(int _len, yosupo::modint61 _h) : len(_len), h(_h) {}
    RollingHash(int c) : len(1), h(c) {}
    // H(l) + H(r) = H(lr)
    RollingHash operator+(const RollingHash& r) const {
        return RollingHash{len + r.len, h + r.h * pow_base[len]};
    }
    RollingHash& operator+=(const RollingHash& r) { return *this = *this + r; }

    bool operator==(const RollingHash& r) const { return len == r.len && h == r.h; }
    bool operator!=(const RollingHash& r) const { return !(*this == r); }
    // H(lr).strip_left(H(l)) = H(r)
    RollingHash strip_left(const RollingHash& l) const {
        return RollingHash{len - l.len, (h - l.h) * pow_ibase[l.len]};
    }
    // H(lr).strip_right(H(r)) = H(l)
    RollingHash strip_right(const RollingHash& r) const {
        return RollingHash{len - r.len, h - r.h * pow_base[len - r.len]};
    }

  private:
    static modint61 base;
    static modint61 ibase;
    static std::vector<modint61> pow_base, pow_ibase;
};
modint61 RollingHash::base = modint61(uniform(0LL, modint61::mod() - 1));
modint61 RollingHash::ibase = RollingHash::base.inv();
std::vector<modint61> RollingHash::pow_base;
std::vector<modint61> RollingHash::pow_ibase;

std::ostream& operator<<(std::ostream& os, const RollingHash& x) {
    return os << "RH(" << x.len << ", " << x.h << ")";
}

namespace internal {

template <class H>
auto update(const H& h, const RollingHash& x) {
    return update(update(h, x.len), x.h);
}

}  // namespace internal

}
