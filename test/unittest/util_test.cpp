#include "yosupo/util.hpp"

#include <ranges>
#include <vector>

#include "gtest/gtest.h"

TEST(UtilTest, ToVec) {
    ASSERT_EQ(std::vector({1, 2, 3}), yosupo::to_vec(std::views::iota(1, 4)));
}

TEST(UtilTest, Dedup) {
    std::vector<int> v1;
    yosupo::dedup(v1);
    ASSERT_EQ(std::vector<int>(), v1);

    std::vector<int> v2 = {1, 2, 3};
    yosupo::dedup(v2);
    ASSERT_EQ(std::vector<int>({1, 2, 3}), v2);

    std::vector<int> v3 = {1, 1, 2, 2, 2, 3, 1, 1};
    yosupo::dedup(v3);
    ASSERT_EQ(std::vector<int>({1, 2, 3, 1}), v3);

    std::vector<int> v4 = {1, 1, 1, 1};
    yosupo::dedup(v4);
    ASSERT_EQ(std::vector<int>({1}), v4);

    std::vector<int> v5 = {1, 4, 2, 8, 5, 7};
    yosupo::dedup(v5, [](int a, int b) { return (a % 3) == (b % 3); });
    ASSERT_EQ(std::vector<int>({1, 2, 7}), v5);
}

TEST(UtilTest, Subspan) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};

    auto s = yosupo::subspan<3>(std::span{v}, 2);
    ASSERT_EQ(std::vector<int>({3, 4, 5}), std::vector(s.begin(), s.end()));
}
