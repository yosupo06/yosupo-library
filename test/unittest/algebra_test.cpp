#include "yosupo/algebra.hpp"

#include "gtest/gtest.h"
#include "yosupo/modint.hpp"

using namespace yosupo;

static_assert(monoid<NoOpMonoid>);
static_assert(monoid<Max<int>>);
static_assert(monoid<Min<int>>);
static_assert(monoid<Sum<int>>);
static_assert(monoid<Prod<int>>);

using mint = ModInt<998244353>;

TEST(AlgebraTEST, ReversibleMonoid) {
    struct S {
        mint a, b;
    };
    auto mo = ReversibleMonoid(Monoid(S(mint(1), mint(0)), [](S a, S b) {
        return S(a.a * b.a, a.b + a.a * b.b);
    }));

    S x = {mint(10), mint(1)};
    S y = {mint(10), mint(2)};
    S z = {mint(10), mint(3)};

    auto w = mo.op(mo.op({x, x}, {y, y}), {z, z});
    EXPECT_EQ(w.val.a, mint(1000));
    EXPECT_EQ(w.val.b, mint(321));
    EXPECT_EQ(w.rev.a, mint(1000));
    EXPECT_EQ(w.rev.b, mint(123));
}
