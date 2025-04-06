#include "yosupo/toptree.hpp"

#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"
#include "yosupo/tree.hpp"

struct TopTreeDP {
    using Point = int;
    using Path = int;
    using Vertex = int;

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
    yosupo::StaticTopTree<TopTreeDP> top_tree(std::move(tree).build(0),
                                              std::vector<int>(5));

    auto path = top_tree.path_prod(0);
    EXPECT_EQ(path, 5);
}

TEST(StaticTopTree, AllProd) {
    yosupo::RootedTreeBuilder tree(5);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(2, 3);
    tree.add_edge(2, 4);
    TopTreeDP dp;
    yosupo::StaticTopTree<TopTreeDP> top_tree(std::move(tree).build(0),
                                              std::vector<int>(5));

    auto all = top_tree.all_prod();
    EXPECT_EQ(all, 5);
}
