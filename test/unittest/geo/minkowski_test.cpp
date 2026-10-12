#include "yosupo/geo/minkowski.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <vector>

#include "yosupo/geo/convex.hpp"
#include "yosupo/int_n.hpp"

using namespace yosupo;

template <typename T> class GeoMinkowskiTest : public ::testing::Test {};
using MinkowskiTypes = ::testing::Types<int, __int128, IntN<10>>;
TYPED_TEST_SUITE(GeoMinkowskiTest, MinkowskiTypes);

TYPED_TEST(GeoMinkowskiTest, EmptyAndPoints) {
    using P = Point<TypeParam>;
    EXPECT_EQ(minkowski_sum(Polygon<TypeParam>{}, Polygon<TypeParam>{{1, 2}}),
              Polygon<TypeParam>{});
    EXPECT_EQ(minkowski_sum(Polygon<TypeParam>{{1, 2}}, Polygon<TypeParam>{}),
              Polygon<TypeParam>{});
    EXPECT_EQ(minkowski_sum(Polygon<TypeParam>{{1, 2}},
                            Polygon<TypeParam>{{3, -4}}),
              (std::vector<P>{{4, -2}}));
    Polygon<TypeParam> triangle = {{0, 0}, {3, 0}, {0, 2}};
    EXPECT_EQ(minkowski_sum(triangle, Polygon<TypeParam>{{-1, 5}}),
              (std::vector<P>{{-1, 5}, {2, 5}, {-1, 7}}));
}

TYPED_TEST(GeoMinkowskiTest, SegmentsAndParallelEdges) {
    using Pol = Polygon<TypeParam>;
    EXPECT_EQ(minkowski_sum(Pol{{0, 0}, {2, 0}}, Pol{{0, 0}, {0, 3}}),
              (Pol{{0, 0}, {2, 0}, {2, 3}, {0, 3}}));
    EXPECT_EQ(minkowski_sum(Pol{{2, 0}, {0, 0}}, Pol{{-1, 0}, {3, 0}}),
              (Pol{{-1, 0}, {5, 0}}));
    EXPECT_EQ(minkowski_sum(Pol{{0, 0}, {2, 0}, {2, 2}, {0, 2}},
                            Pol{{1, 1}, {4, 1}, {4, 4}, {1, 4}}),
              (Pol{{1, 1}, {6, 1}, {6, 6}, {1, 6}}));
}

TYPED_TEST(GeoMinkowskiTest, RandomAgainstPairwiseSums) {
    using P = Point<TypeParam>;
    std::mt19937 rng(12345);
    for (int trial = 0; trial < 300; trial++) {
        auto make_hull = [&]() {
            std::vector<P> ps;
            int size = int(rng() % 15);
            for (int i = 0; i < size; i++) {
                ps.emplace_back(int(rng() % 21) - 10, int(rng() % 21) - 10);
            }
            return convex(ps);
        };
        auto a = make_hull(), b = make_hull();
        std::vector<P> sums;
        for (auto p : a) {
            for (auto q : b) sums.push_back(p + q);
        }
        auto expected = convex(sums);
        if (!a.empty()) {
            std::rotate(a.begin(), a.begin() + int(rng() % a.size()), a.end());
        }
        if (!b.empty()) {
            std::rotate(b.begin(), b.begin() + int(rng() % b.size()), b.end());
        }
        EXPECT_EQ(minkowski_sum(a, b), expected);
        EXPECT_EQ(minkowski_sum(b, a), expected);
    }
}
