#include "yosupo/geo/line.hpp"
#include "yosupo/fraction.hpp"

#include "gtest/gtest.h"

using namespace yosupo;

using R = Fraction<int>;
using P = Point<R>;
using L = Line<R>;

TEST(LineTest, CrossLLIntersecting) {
    L l1{P{0, 0}, P{1, 1}};
    L l2{P{0, 1}, {1, 0}};
    auto res1 = cross_ll(l1, l2);
    ASSERT_EQ(res1.first, 1);
    ASSERT_EQ(res1.second.x, R(1, 2));
    ASSERT_EQ(res1.second.y, R(1, 2));
}
TEST(LineTest, CrossLLParallel) {
    L l3{{0, 0}, {1, 0}};
    L l4{{0, 1}, {1, 1}};
    auto res2 = cross_ll(l3, l4);
    ASSERT_EQ(res2.first, 0);
}
TEST(LineTest, CrossLLCollinear) {
    L l5{{0, 0}, {1, 1}};
    L l6{{2, 2}, {3, 3}};
    auto res3 = cross_ll(l5, l6);
    ASSERT_EQ(res3.first, -1);
}

TEST(LineTest, HalfplaneIntersectsEmpty) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{10, -1}, {0, -1}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(res.empty());
}
