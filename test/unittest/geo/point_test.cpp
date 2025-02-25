#include "../../utils/int128.hpp"  // IWYU pragma: keep

#include "yosupo/geo/point.hpp"
#include "yosupo/int_n.hpp"

#include "gtest/gtest.h"

using namespace yosupo;

using PointTypes =
    ::testing::Types<Point<int>, Point<__int128>, Point<IntN<10>>>;

template <typename T> class GeoPointTest : public ::testing::Test {};

TYPED_TEST_SUITE(GeoPointTest, PointTypes);

TYPED_TEST(GeoPointTest, DefaultConstructor) {
    TypeParam p;
    EXPECT_EQ(p.x, 0);
    EXPECT_EQ(p.y, 0);
}

TYPED_TEST(GeoPointTest, Add) {
    EXPECT_EQ(TypeParam(1, 2) + TypeParam(3, 4), TypeParam(4, 6));
    TypeParam p = TypeParam(1, 2);
    p += TypeParam(3, 4);
    EXPECT_EQ(p, TypeParam(4, 6));
}

TYPED_TEST(GeoPointTest, Multiply) {
    EXPECT_EQ(TypeParam(1, 2) * TypeParam(3, 4), TypeParam(-5, 10));
    TypeParam p = TypeParam(1, 2);
    p *= TypeParam(3, 4);
    EXPECT_EQ(p, TypeParam(-5, 10));
}

TYPED_TEST(GeoPointTest, MultiplyScalar) {
    EXPECT_EQ(TypeParam(1, 2) * 3, TypeParam(3, 6));
    EXPECT_EQ(3 * TypeParam(1, 2), TypeParam(3, 6));
    TypeParam p = TypeParam(1, 2);
    p *= 3;
    EXPECT_EQ(p, TypeParam(3, 6));
}

TYPED_TEST(GeoPointTest, DivideScalar) {
    EXPECT_EQ(TypeParam(3, 6) / 3, TypeParam(1, 2));
    TypeParam p = TypeParam(3, 6);
    p /= 3;
    EXPECT_EQ(p, TypeParam(1, 2));
}

TYPED_TEST(GeoPointTest, Subtract) {
    EXPECT_EQ(TypeParam(1, 2) - TypeParam(3, 4), TypeParam(-2, -2));
    TypeParam p = TypeParam(1, 2);
    p -= TypeParam(3, 4);
    EXPECT_EQ(p, TypeParam(-2, -2));
}

TYPED_TEST(GeoPointTest, UnaryMinus) {
    EXPECT_EQ(-TypeParam(1, 2), TypeParam(-1, -2));
}

TYPED_TEST(GeoPointTest, Compare) {
    EXPECT_LT(TypeParam(1, 2), TypeParam(3, 4));
    EXPECT_EQ(TypeParam(1, 2), TypeParam(1, 2));
    EXPECT_GT(TypeParam(3, 4), TypeParam(1, 2));

    EXPECT_LT(TypeParam(1, 2), TypeParam(1, 3));
    EXPECT_LT(TypeParam(1, 2), TypeParam(2, 1));
    EXPECT_LT(TypeParam(1, 2), TypeParam(2, 2));
}

TYPED_TEST(GeoPointTest, CrossProduct) {
    EXPECT_EQ(crs(TypeParam(1, 2), TypeParam(3, 4)), -2);
    EXPECT_EQ(crs(TypeParam(3, 4), TypeParam(1, 2)), 2);
}

TYPED_TEST(GeoPointTest, DotProduct) {
    EXPECT_EQ(dot(TypeParam(1, 2), TypeParam(3, 4)), 11);
    EXPECT_EQ(dot(TypeParam(3, 4), TypeParam(1, 2)), 11);
}

TYPED_TEST(GeoPointTest, ToString) {
    {
        std::ostringstream oss;
        oss << TypeParam(1, 2);
        EXPECT_EQ(oss.str(), "P(1, 2)");
    }
    {
        std::ostringstream oss;
        oss << TypeParam(3, 4);
        EXPECT_EQ(oss.str(), "P(3, 4)");
    }
}

TYPED_TEST(GeoPointTest, CCW) {
    // on
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(0, 0)), 0);
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(2, 3)), 0);
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(4, 6)), 0);

    // front
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(40, 60)), -2);

    // back
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(-2, -3)), 2);

    // cclock
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(3, 6)), 1);

    // clock
    EXPECT_EQ(ccw(TypeParam(4, 6), TypeParam(5, 6)), -1);

    // three points
    EXPECT_EQ(ccw(TypeParam(0, 0), TypeParam(1, 0), TypeParam(0, 1)), 1);
    EXPECT_EQ(ccw(TypeParam(1, 1), TypeParam(2, 1), TypeParam(1, 2)), 1);
}

TYPED_TEST(GeoPointTest, CmpArg) {
    EXPECT_TRUE(cmp_arg(TypeParam(0, 0), TypeParam(1, 0)) == 0);
    EXPECT_TRUE(cmp_arg(TypeParam(-1, 0), TypeParam(0, 0)) > 0);
    EXPECT_TRUE(cmp_arg(TypeParam(-1, 1), TypeParam(1, 1)) > 0);
    EXPECT_TRUE(cmp_arg(TypeParam(1, 1), TypeParam(1, -1)) > 0);
    EXPECT_TRUE(cmp_arg(TypeParam(1, -1), TypeParam(-1, -1)) > 0);
}
