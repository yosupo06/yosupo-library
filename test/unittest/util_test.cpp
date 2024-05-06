#include "yosupo/util.hpp"

#include <vector>

#include "gtest/gtest.h"

TEST(UtilTest, ToVec) {
    ASSERT_EQ(std::vector({1, 2, 3}), yosupo::to_vec(std::views::iota(1, 4)));
}
