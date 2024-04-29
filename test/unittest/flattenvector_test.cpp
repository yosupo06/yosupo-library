#include "yosupo/flattenvector.hpp"

#include <numeric>
#include <vector>
#include <queue>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(FlattenVectorTest, Usage) {
    FlattenVector v(3, std::vector<std::pair<int, int>>({{1, 0}, {2, 0}, {10, 1}, {20, 2}, {11, 1}}));
    ASSERT_EQ(v.start, std::vector({0, 2, 4, 5}));
    ASSERT_EQ(v.v, std::vector({1, 2, 10, 11, 20}));
}
