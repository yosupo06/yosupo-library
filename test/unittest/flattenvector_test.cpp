#include "yosupo/flattenvector.hpp"
#include "yosupo/util.hpp"

#include <utility>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(FlattenVectorTest, Usage) {
    FlattenVector v(3, std::vector<std::pair<int, int>>(
                           {{0, 1}, {0, 2}, {1, 10}, {2, 20}, {1, 11}}));
    ASSERT_EQ(v.start, std::vector({0, 2, 4, 5}));
    ASSERT_EQ(v.v, std::vector({1, 2, 10, 11, 20}));
}

TEST(FlattenVectorTest, At) {
    FlattenVector v(3, std::vector<std::pair<int, int>>(
                           {{0, 1}, {0, 2}, {1, 10}, {2, 20}, {1, 11}}));
    ASSERT_EQ(to_vec(v.at(0)), std::vector({1, 2}));
    ASSERT_EQ(to_vec(v.at(1)), std::vector({10, 11}));
    ASSERT_EQ(to_vec(v.at(2)), std::vector({20}));
}

TEST(FlattenVectorTest, EraseIf) {
    FlattenVector<int> v(3, std::vector<std::pair<int, int>>(
                                {{0, 1}, {0, 2}, {1, 10}, {2, 20}, {1, 11}}));

    int removed = v.erase_if(
        [](const std::pair<int, int>& p) { return p.second % 2 == 0; });
    EXPECT_EQ(removed, 3);
    EXPECT_EQ(v.start, std::vector({0, 1, 2, 2}));
    EXPECT_EQ(v.v, std::vector({1, 11}));

    removed =
        v.erase_if([](const std::pair<int, int>& p) { return p.first == 1; });
    EXPECT_EQ(removed, 1);
    EXPECT_EQ(v.start, std::vector({0, 1, 1, 1}));
    EXPECT_EQ(v.v, std::vector({1}));

    removed = v.erase_if([](const std::pair<int, int>&) { return true; });
    EXPECT_EQ(removed, 1);
    EXPECT_EQ(v.start, std::vector({0, 0, 0, 0}));
    EXPECT_EQ(v.v, std::vector<int>());
}
