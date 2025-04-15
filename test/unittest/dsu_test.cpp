#include "yosupo/dsu.hpp"

#include "gtest/gtest.h"

TEST(DsuTest, Usage) {
    yosupo::RollbackDSU d(2);
    EXPECT_EQ(0, d.leader(0));
    EXPECT_EQ(1, d.leader(1));
    EXPECT_FALSE(d.same(0, 1));
    EXPECT_EQ(1, d.size(0));
    EXPECT_EQ(1, d.size(1));
    int root = -1;
    d.with([&]() {
        root = d.merge(0, 1);
        EXPECT_EQ(root, d.leader(0));
        EXPECT_EQ(root, d.leader(1));
        EXPECT_TRUE(d.same(0, 1));
        EXPECT_EQ(2, d.size(0));
        EXPECT_EQ(2, d.size(1));
    });
    EXPECT_EQ(0, d.leader(0));
    EXPECT_EQ(1, d.leader(1));
    EXPECT_FALSE(d.same(0, 1));
    EXPECT_EQ(1, d.size(0));
    EXPECT_EQ(1, d.size(1));
}
