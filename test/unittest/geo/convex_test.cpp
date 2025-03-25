#include "yosupo/geo/convex.hpp"

#include <gtest/gtest.h>

#include <vector>

#include "yosupo/geo/point.hpp"
#include "yosupo/int_n.hpp"

using namespace yosupo;

using PointTypes =
    ::testing::Types<Point<int>, Point<__int128>, Point<IntN<10>>>;

template <typename T> class GeoConvexTest : public ::testing::Test {};

TYPED_TEST_SUITE(GeoConvexTest, PointTypes);

TYPED_TEST(GeoConvexTest, Empty) {
    std::vector<TypeParam> points;
    auto hull = yosupo::convex(points);
    EXPECT_EQ(hull, std::vector<TypeParam>({}));
}

TYPED_TEST(GeoConvexTest, Single) {
    std::vector<TypeParam> points = {TypeParam(1, 2)};
    auto hull = yosupo::convex(points);
    EXPECT_EQ(hull, std::vector<TypeParam>({TypeParam(1, 2)}));
}

TYPED_TEST(GeoConvexTest, TwoPoints) {
    std::vector<TypeParam> points = {TypeParam(1, 2), TypeParam(3, 4)};
    auto hull = yosupo::convex(points);
    EXPECT_EQ(hull, std::vector<TypeParam>({TypeParam(1, 2), TypeParam(3, 4)}));
}

TYPED_TEST(GeoConvexTest, Triangle) {
    std::vector<TypeParam> points = {TypeParam(1, 1), TypeParam(10, 1),
                                     TypeParam(5, 10)};
    auto hull = yosupo::convex(points);
    EXPECT_EQ(hull, std::vector<TypeParam>(
                        {TypeParam(1, 1), TypeParam(10, 1), TypeParam(5, 10)}));
}

TYPED_TEST(GeoConvexTest, TriangleOrder) {
    std::vector<TypeParam> points = {TypeParam(10, 1), TypeParam(5, 10),
                                     TypeParam(1, 1)};
    auto hull = yosupo::convex(points);
    EXPECT_EQ(hull, std::vector<TypeParam>(
                        {TypeParam(1, 1), TypeParam(10, 1), TypeParam(5, 10)}));
}

TYPED_TEST(GeoConvexTest, RemoveInside) {
    std::vector<TypeParam> points = {TypeParam(1, 1), TypeParam(10, 1),
                                     TypeParam(5, 10), TypeParam(5, 5)};
    auto hull = yosupo::convex(points);
    EXPECT_EQ(hull, std::vector<TypeParam>(
                        {TypeParam(1, 1), TypeParam(10, 1), TypeParam(5, 10)}));
}
