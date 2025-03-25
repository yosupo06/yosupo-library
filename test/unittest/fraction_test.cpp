#include "yosupo/fraction.hpp"

#include "gtest/gtest.h"
#include "yosupo/int_n.hpp"
#include "yosupo/math.hpp"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

using ValueTypes = ::testing::Types<int, __int128, IntN<10>>;

template <typename T> class FractionTest : public ::testing::Test {};

TYPED_TEST_SUITE(FractionTest, ValueTypes);

TYPED_TEST(FractionTest, Constructor) {
    EXPECT_EQ(Fraction<TypeParam>(), Fraction<TypeParam>(0, 1));
    EXPECT_EQ(Fraction<TypeParam>(3), Fraction<TypeParam>(3, 1));
    EXPECT_EQ(Fraction<TypeParam>(12, 20), Fraction<TypeParam>(3, 5));
}

TYPED_TEST(FractionTest, Add) {
    Fraction<TypeParam> f(3, 5);
    Fraction<TypeParam> g(2, 3);
    EXPECT_EQ(f + g, Fraction<TypeParam>(19, 15));
}

TYPED_TEST(FractionTest, Comp) {
    EXPECT_LT(Fraction<TypeParam>(-1, 3), Fraction<TypeParam>(-1, 5));
    EXPECT_LT(Fraction<TypeParam>(1, 5), Fraction<TypeParam>(1, 3));
}

TYPED_TEST(FractionTest, Dump) {
    Fraction<TypeParam> f(3, 5);
    EXPECT_EQ(f.dump(), "3/5");
    Fraction<TypeParam> f2(3, 1);
    EXPECT_EQ(f2.dump(), "3");
}

TYPED_TEST(FractionTest, Sign) {
    EXPECT_EQ(Fraction<TypeParam>(-3, 5).sgn(), -1);
    EXPECT_EQ(Fraction<TypeParam>(0, 5).sgn(), 0);
    EXPECT_EQ(Fraction<TypeParam>(3, 5).sgn(), 1);
}

TYPED_TEST(FractionTest, Abs) {
    EXPECT_EQ(abs(Fraction<TypeParam>(3, 5)), Fraction<TypeParam>(3, 5));
    EXPECT_EQ(abs(Fraction<TypeParam>(-3, 5)), Fraction<TypeParam>(3, 5));
}
