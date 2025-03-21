#include "yosupo/toptree.hpp"
#include <utility>
#include "yosupo/algebra.hpp"

#include "gtest/gtest.h"
#include "yosupo/tree.hpp"

struct TopTreeDP {
    using Point = int;
    using Path = int;

    yosupo::Sum<int> point = yosupo::Sum<int>(0);
    yosupo::Sum<int> path = yosupo::Sum<int>(0);

    Path add_vertex(Point x, int) { return x + 1; }
    Point add_edge(Path x) { return x; }
};

TEST(StaticTopTree, Usage) {
    yosupo::RootedTreeBuilder tree(5);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(2, 3);
    tree.add_edge(2, 4);
    TopTreeDP dp;
    yosupo::StaticTopTree top_tree(std::move(tree).build(0), dp);

    auto path = top_tree.path_prod(0);
    EXPECT_EQ(path, 5);
}
