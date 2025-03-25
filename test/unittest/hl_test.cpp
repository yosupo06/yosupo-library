#include "yosupo/hl.hpp"

#include <utility>

#include "gtest/gtest.h"
#include "yosupo/tree.hpp"

using namespace yosupo;

TEST(HLTest, Subtree) {
    RootedTreeBuilder tree(5);
    tree.add_edge(0, 1);
    tree.add_edge(0, 2);
    tree.add_edge(1, 3);
    tree.add_edge(1, 4);
    HLEulerTour hl(std::move(tree).build(0));

    ASSERT_EQ(hl.subtree_size(0), 5);
    ASSERT_EQ(hl.subtree_size(1), 3);
    ASSERT_EQ(hl.subtree_size(2), 1);
    ASSERT_EQ(hl.subtree_size(3), 1);
    ASSERT_EQ(hl.subtree_size(4), 1);
}

TEST(HLTest, N1) {
    RootedTreeBuilder tree(1);
    HLEulerTour hl(std::move(tree).build(0));
}
