#include "yosupo/geo/polygon.hpp"

#include <gtest/gtest.h>

#include "yosupo/geo/point.hpp"
#include "yosupo/int_n.hpp"

using namespace yosupo;

using ValueTypes = ::testing::Types<int, __int128, IntN<10>>;

template <typename T> class GeoPolygonTest : public ::testing::Test {};

TYPED_TEST_SUITE(GeoPolygonTest, ValueTypes);

TYPED_TEST(GeoPolygonTest, Contains) {
    using P = Point<TypeParam>;
    Polygon<TypeParam> pol = {P(1, 1), P(10, 1), P(5, 10)};

    EXPECT_EQ(contains(pol, P(5, 5)), 1);   // Inside
    EXPECT_EQ(contains(pol, P(0, 0)), -1);  // Outside
    EXPECT_EQ(contains(pol, P(1, 1)), 0);   // On vertex
    EXPECT_EQ(contains(pol, P(6, 1)), 0);   // On edge
}

TYPED_TEST(GeoPolygonTest, ContainsTricky) {
    using P = Point<TypeParam>;
    Polygon<TypeParam> pol = {
        P(0, 0),  P(15, 0), P(15, 10), P(10, 10),
        P(10, 5), P(5, 5),  P(5, 10),  P(0, 10),
    };

    EXPECT_EQ(contains(pol, P(1, 1)), 1);
    EXPECT_EQ(contains(pol, P(2, 5)), 1);
    EXPECT_EQ(contains(pol, P(7, 5)), 0);
    EXPECT_EQ(contains(pol, P(7, 6)), -1);
    EXPECT_EQ(contains(pol, P(12, 5)), 1);
}
