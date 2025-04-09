#include "yosupo/util.hpp"

#include <ranges>
#include <span>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;

TEST(UtilTest, Chmin) {
    int a = 10;
    EXPECT_TRUE(chmin(a, 5));
    EXPECT_EQ(5, a);
    EXPECT_FALSE(chmin(a, 7));
    EXPECT_EQ(5, a);
    EXPECT_FALSE(chmin(a, 5));
    EXPECT_EQ(5, a);
}

TEST(UtilTest, Chmax) {
    int a = 10;
    EXPECT_TRUE(chmax(a, 15));
    EXPECT_EQ(15, a);
    EXPECT_FALSE(chmax(a, 12));
    EXPECT_EQ(15, a);
    EXPECT_FALSE(chmax(a, 15));
    EXPECT_EQ(15, a);
}

TEST(UtilTest, FloorDiv) {
    EXPECT_EQ(floor_div(11, 5), 2);
    EXPECT_EQ(floor_div(10, 5), 2);
    EXPECT_EQ(floor_div(9, 5), 1);
    EXPECT_EQ(floor_div(-9, 5), -2);
    EXPECT_EQ(floor_div(-10, 5), -2);
    EXPECT_EQ(floor_div(-11, 5), -3);
}

TEST(UtilTest, CeilDiv) {
    EXPECT_EQ(ceil_div(11, 5), 3);
    EXPECT_EQ(ceil_div(10, 5), 2);
    EXPECT_EQ(ceil_div(9, 5), 2);
    EXPECT_EQ(ceil_div(-9, 5), -1);
    EXPECT_EQ(ceil_div(-10, 5), -2);
    EXPECT_EQ(ceil_div(-11, 5), -2);
}

TEST(UtilTest, ToVec) {
    EXPECT_EQ(std::vector({1, 2, 3}), to_vec(std::views::iota(1, 4)));
}

TEST(UtilTest, Dedup) {
    std::vector<int> v1;
    dedup(v1);
    EXPECT_EQ(std::vector<int>(), v1);

    std::vector<int> v2 = {1, 2, 3};
    dedup(v2);
    EXPECT_EQ(std::vector<int>({1, 2, 3}), v2);

    std::vector<int> v3 = {1, 1, 2, 2, 2, 3, 1, 1};
    dedup(v3);
    EXPECT_EQ(std::vector<int>({1, 2, 3, 1}), v3);

    std::vector<int> v4 = {1, 1, 1, 1};
    dedup(v4);
    EXPECT_EQ(std::vector<int>({1}), v4);

    std::vector<int> v5 = {1, 4, 2, 8, 5, 7};
    dedup(v5, [](int a, int b) { return (a % 3) == (b % 3); });
    EXPECT_EQ(std::vector<int>({1, 2, 7}), v5);
}

TEST(UtilTest, Subspan) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};

    auto s = yosupo::subspan<3>(std::span{v}, 2);
    EXPECT_EQ(std::vector<int>({3, 4, 5}), std::vector(s.begin(), s.end()));
}

TEST(UtilTest, Rep) {
    EXPECT_EQ(std::vector<int>({0, 1, 2}), to_vec(yosupo::rep(0, 3)));
    EXPECT_EQ(std::vector<int>({-2, -1, 0, 1}), to_vec(yosupo::rep(-2, 2)));
    EXPECT_EQ(std::vector<int>({5}), yosupo::to_vec(rep(5, 6)));
    EXPECT_EQ(std::vector<int>(), yosupo::to_vec(rep(0, 0)));  // l == r case
    EXPECT_EQ(std::vector<int>(), yosupo::to_vec(rep(5, 2)));  // l > r case
}
