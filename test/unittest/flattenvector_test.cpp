#include "yosupo/flattenvector.hpp"

#include <numeric>
#include <queue>
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
