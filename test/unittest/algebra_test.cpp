#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/modint.hpp"

#include "gtest/gtest.h"

using namespace yosupo;

static_assert(monoid<Max<int>>);
static_assert(monoid<Min<int>>);
static_assert(monoid<Sum<int>>);
static_assert(monoid<Prod<int>>);

using mint = ModInt<998244353>;

struct ModAffine {
    struct S {
        // f(x) = a * x + b
        mint a, b;
    };
    // op(l, r) = fl(fr(x))
    S op(S l, S r) { return S(l.a * r.a, l.b + l.a * r.b); }
    S e = S(mint(1), mint(0));
};

TEST(AlgebraTEST, ReversibleMonoid) {
    auto mo = ReversibleMonoid<ModAffine>(ModAffine());

    ModAffine::S x = {mint(10), mint(1)};
    ModAffine::S y = {mint(10), mint(2)};
    ModAffine::S z = {mint(10), mint(3)};

    auto w = mo.op(mo.op({x, x}, {y, y}), {z, z});
    EXPECT_EQ(w.val.a, mint(1000));
    EXPECT_EQ(w.val.b, mint(321));
    EXPECT_EQ(w.rev.a, mint(1000));
    EXPECT_EQ(w.rev.b, mint(123));
}
