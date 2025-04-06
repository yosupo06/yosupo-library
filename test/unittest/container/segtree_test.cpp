#include "yosupo/container/segtree.hpp"

#include <array>
#include <ranges>
#include <vector>

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

TEST(SegTreeTest, SizeConstructor) {
    SegTree seg(5, Sum<ll>(0));
    EXPECT_EQ(seg.all_prod(), 0);
}

TEST(SegTreeTest, RangeConstructor) {
    // std::array
    std::array<ll, 5> arr = {1, 2, 3, 4, 5};
    SegTree seg_arr(arr, Sum<ll>(0));
    EXPECT_EQ(seg_arr.all_prod(), 15);
    EXPECT_EQ(seg_arr.get(4), 5);
    EXPECT_EQ(seg_arr.prod(1, 3), 5);
    seg_arr.set(2, 10);
    EXPECT_EQ(seg_arr.all_prod(), 22);
    EXPECT_EQ(seg_arr.prod(1, 3), 12);

    // std::vector view
    std::vector<ll> vec = {10, 20, 30};
    SegTree seg_view(std::views::all(vec), Sum<ll>(0));
    EXPECT_EQ(seg_view.all_prod(), 60);
    EXPECT_EQ(seg_view.get(1), 20);
    EXPECT_EQ(seg_view.prod(0, 2), 30);
    seg_view.set(0, 100);
    EXPECT_EQ(seg_view.all_prod(), 150);
    EXPECT_EQ(seg_view.prod(0, 2), 120);

    // Empty range
    std::vector<ll> empty_vec;
    SegTree seg_empty(empty_vec, Sum<ll>(0));
    EXPECT_EQ(seg_empty.all_prod(), 0);
    EXPECT_EQ(seg_empty.prod(0, 0), 0);

    // Empty array
    std::array<ll, 0> empty_arr;
    SegTree seg_empty_arr(empty_arr, Sum<ll>(0));
    EXPECT_EQ(seg_empty_arr.all_prod(), 0);
    EXPECT_EQ(seg_empty_arr.prod(0, 0), 0);
}
