#include "yosupo/hl.hpp"
#include "gtest/gtest.h"

using namespace yosupo;

TEST(HLTest, Subtree) {
    HLEulerTour hl(5, 0);
    hl.add_edge(0, 1);
    hl.add_edge(0, 2);
    hl.add_edge(1, 3);
    hl.add_edge(1, 4);

    hl.build();

    ASSERT_EQ(hl.subtree_size(0), 5);
    ASSERT_EQ(hl.subtree_size(1), 3);
    ASSERT_EQ(hl.subtree_size(2), 1);
    ASSERT_EQ(hl.subtree_size(3), 1);
    ASSERT_EQ(hl.subtree_size(4), 1);
}

TEST(HLTest, N1) {
    HLEulerTour hl(1, 0);
    hl.build();
}
