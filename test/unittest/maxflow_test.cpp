#include "yosupo/maxflow.hpp"

#include <algorithm>
#include <numeric>

#include "gtest/gtest.h"

TEST(MaxFlowTest, usage) {
    yosupo::MFWithLowerCapGraph<int> g(2);
    g.add_edge(0, 1, 10, 100);
    ASSERT_EQ(100, g.max_flow(0, 1));
}

TEST(MaxFlowTest, invalid) {
    yosupo::MFWithLowerCapGraph<int> g(3);
    g.add_edge(0, 1, 10, 100);
    g.add_edge(1, 2, 1000, 10000);
    ASSERT_EQ(-1, g.max_flow(0, 2));
}
