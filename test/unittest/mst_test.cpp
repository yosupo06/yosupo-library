#include "yosupo/mst.hpp"

#include "gtest/gtest.h"

using namespace yosupo;

TEST(MSTTest, Usage) {
    MST<int> mst(4);
    mst.add_edge(0, 1, 2);
    mst.add_edge(1, 3, 4);
    mst.add_edge(1, 3, 3);
    auto result = mst.mst();
    ASSERT_EQ(result.first, 5);
    ASSERT_EQ(result.second, std::vector<int>({0, 2}));
}
