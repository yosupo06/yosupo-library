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
    auto mo = ReversibleMonoid(Monoid(
        S(mint(1), mint(0)),
        [](const S& a, const S& b) { return S(a.a * b.a, a.b + a.a * b.b); }));

    S x = {mint(10), mint(1)};
    S y = {mint(10), mint(2)};
    S z = {mint(10), mint(3)};

    auto w = mo.op(mo.op({x, x}, {y, y}), {z, z});
    EXPECT_EQ(w.val.a, mint(1000));
    EXPECT_EQ(w.val.b, mint(321));
    EXPECT_EQ(w.rev.a, mint(1000));
    EXPECT_EQ(w.rev.b, mint(123));
}

TEST(AlgebraTest, MaxMonoid) {
    Max<int> mo;
    EXPECT_EQ(mo.op(1, 2), 2);
    EXPECT_EQ(mo.op(2, 1), 2);
    EXPECT_EQ(mo.op(2, 2), 2);
}

TEST(AlgebraTest, MinMonoid) {
    Min<int> mo;
    EXPECT_EQ(mo.op(1, 2), 1);
    EXPECT_EQ(mo.op(2, 1), 1);
    EXPECT_EQ(mo.op(2, 2), 2);
}

TEST(AlgebraTest, SumMonoid) {
    Sum<int> mo(0);
    EXPECT_EQ(mo.op(1, 2), 3);
    EXPECT_EQ(mo.op(2, 1), 3);
    EXPECT_EQ(mo.op(2, 2), 4);
}

TEST(AlgebraTest, ProdMonoid) {
    Prod<int> mo(1);
    EXPECT_EQ(mo.op(1, 2), 2);
    EXPECT_EQ(mo.op(2, 1), 2);
    EXPECT_EQ(mo.op(2, 2), 4);
}
