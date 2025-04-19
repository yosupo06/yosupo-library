#include "yosupo/container/segtree2d.hpp"

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(SegTree2DTest, Usage) {
    SegTree2D seg({3, 3}, Sum<ll>(0));
    EXPECT_EQ(seg.all_prod(), 0);

    seg.add({1, 2}, 1);
    EXPECT_EQ(seg.get({0, 0}), 0);
    EXPECT_EQ(seg.get({1, 2}), 1);
    EXPECT_EQ(seg.prod({0, 0}, {3, 3}), 1);
    seg.add({2, 1}, 10);
    EXPECT_EQ(seg.prod({0, 0}, {3, 3}), 11);
    EXPECT_EQ(seg.prod({0, 0}, {3, 2}), 10);
    seg.add({1, 2}, 100);
    EXPECT_EQ(seg.prod({0, 0}, {3, 3}), 111);
}
