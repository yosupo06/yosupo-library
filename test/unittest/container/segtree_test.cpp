#include "yosupo/container/segtree.hpp"

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(SegTreeTest, Usage) {
    SegTree seg({1, 2, 3, 4, 5}, Sum<ll>(0));
    EXPECT_EQ(seg.all_prod(), 15);
    EXPECT_EQ(seg.get(4), 5);
    EXPECT_EQ(seg.prod(1, 3), 5);
    seg.set(2, 10);
    EXPECT_EQ(seg.all_prod(), 22);
    EXPECT_EQ(seg.prod(1, 3), 12);
}
