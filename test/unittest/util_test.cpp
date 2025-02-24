#include "yosupo/util.hpp"

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
}
