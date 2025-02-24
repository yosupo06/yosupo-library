#include "yosupo/geo/point.hpp"

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(GeoPointTest, Add) {
    EXPECT_EQ(Point(1, 2) + Point(3, 4), Point(4, 6));
    Point p = Point(1, 2);
    p += Point(3, 4);
    EXPECT_EQ(p, Point(4, 6));
}

TEST(GeoPointTest, Multiply) {
    EXPECT_EQ(Point(1, 2) * Point(3, 4), Point(-5, 10));
    Point p = Point(1, 2);
    p *= Point(3, 4);
    EXPECT_EQ(p, Point(-5, 10));
}

TEST(GeoPointTest, MultiplyScalar) {
    EXPECT_EQ(Point(1, 2) * 3, Point(3, 6));
    EXPECT_EQ(3 * Point(1, 2), Point(3, 6));
    Point p = Point(1, 2);
    p *= 3;
    EXPECT_EQ(p, Point(3, 6));
}

TEST(GeoPointTest, Subtract) {
    EXPECT_EQ(Point(1, 2) - Point(3, 4), Point(-2, -2));
    Point p = Point(1, 2);
    p -= Point(3, 4);
    EXPECT_EQ(p, Point(-2, -2));
}

TEST(GeoPointTest, UnaryMinus) { EXPECT_EQ(-Point(1, 2), Point(-1, -2)); }

TEST(GeoPointTest, Compare) {
    EXPECT_LT(Point(1, 2), Point(3, 4));
    EXPECT_EQ(Point(1, 2), Point(1, 2));
    EXPECT_GT(Point(3, 4), Point(1, 2));

    EXPECT_LT(Point(1, 2), Point(1, 3));
    EXPECT_LT(Point(1, 2), Point(2, 1));
    EXPECT_LT(Point(1, 2), Point(2, 2));
}

TEST(GeoPointTest, CrossProduct) {
    EXPECT_EQ(crs(Point(1, 2), Point(3, 4)), -2);
    EXPECT_EQ(crs(Point(3, 4), Point(1, 2)), 2);
}

TEST(GeoPointTest, DotProduct) {
    EXPECT_EQ(dot(Point(1, 2), Point(3, 4)), 11);
    EXPECT_EQ(dot(Point(3, 4), Point(1, 2)), 11);
}

TEST(GeoPointTest, ToString) {
    {
        std::ostringstream oss;
        oss << Point(1, 2);
        EXPECT_EQ(oss.str(), "P(1, 2)");
    }
    {
        std::ostringstream oss;
        oss << Point(3, 4);
        EXPECT_EQ(oss.str(), "P(3, 4)");
    }
}
