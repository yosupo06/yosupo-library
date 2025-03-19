#include "yosupo/networksimplex.hpp"
#include <utility>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;

TEST(NetworkSimplexTest, Usage) {
    NetworkSimplexGraph<int, int> g(3);
    g.add_edge(0, 1, 0, 3, 1);
    g.add_edge(1, 2, 0, 10, 2);
    g.add_excess(0, 2);
    g.add_excess(2, -2);

    auto r = std::move(g).solve<int>();
    ASSERT_TRUE(r.feasible);
    ASSERT_EQ(r.cost, 6);
    ASSERT_EQ(r.flow, std::vector<int>({2, 2}));
}

TEST(NetworkSimplexTest, Infeasible) {
    NetworkSimplexGraph<int, int> g(3);
    g.add_edge(0, 1, 0, 3, 1);
    g.add_edge(1, 2, 0, 10, 2);
    g.add_excess(0, 10);
    g.add_excess(2, -10);

    auto r = std::move(g).solve<int>();
    ASSERT_FALSE(r.feasible);
}

TEST(NetworkSimplexTest, AddVertex) {
    NetworkSimplexGraph<int, int> g;
    g.add_vertex();
    g.add_vertex();
    g.add_edge(0, 1, 0, 3, 1);
    g.add_vertex();
    g.add_edge(1, 2, 0, 10, 2);
    g.add_excess(0, 10);
    g.add_excess(2, -10);

    auto r = std::move(g).solve<int>();
    ASSERT_FALSE(r.feasible);
}
