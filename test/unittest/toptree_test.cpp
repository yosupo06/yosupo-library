#include <algorithm>
#include <queue>

#include "yosupo/algebra.hpp"
#include "yosupo/toptree.hpp"

#include "gtest/gtest.h"

struct TopTreeDP {
    using Point = int;
    using Path = int;

    yosupo::Sum<int> point = yosupo::Sum<int>(0);
    yosupo::Sum<int> path = yosupo::Sum<int>(0);

    Path add_vertex(Point x, int) { return x + 1; }
    Point add_edge(Path x) { return x; }
};

TEST(StaticTopTree, Usage) {
    TopTreeDP dp;
    yosupo::StaticTopTree top_tree(5, dp);
    top_tree.add_edge(0, 1);
    top_tree.add_edge(1, 2);
    top_tree.add_edge(2, 3);
    top_tree.add_edge(2, 4);
    top_tree.init();

    auto path = top_tree.path_prod(0);
    EXPECT_EQ(path, 5);
}
