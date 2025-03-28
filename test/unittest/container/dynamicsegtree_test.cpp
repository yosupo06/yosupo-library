#include "yosupo/container/dynamicsegtree.hpp"

#include <functional>

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"

TEST(DynamicSegtreeTest, Usage) {
    yosupo::DynamicSegtree tree((yosupo::ActedMonoid(
        yosupo::Max<int>(), yosupo::Monoid<int, std::plus<int>>(0),
        [](int a, int b) { return a + b; })));
}

TEST(DynamicSegtreeTest, NoLazy) {
    yosupo::DynamicSegtree tree((yosupo::ActedMonoid(yosupo::Sum<int>(0))));
    auto tr = tree.build(1 << 30);
    tree.set(tr, 0, 1);
    tree.set(tr, 1, 10);
    tree.set(tr, 1 << 29, 100);
    EXPECT_EQ(tree.all_prod(tr), 111);
    tree.set(tr, 1, 3);
    EXPECT_EQ(tree.all_prod(tr), 104);
    tree.set(tr, 2, 3);
    EXPECT_EQ(tree.all_prod(tr), 107);
}

TEST(DynamicSegtreeTest, SetE) {
    yosupo::DynamicSegtree tree((yosupo::ActedMonoid(yosupo::Sum<int>(0))));
    auto tr = tree.build(1 << 30);
    tree.set(tr, 0, 1);
    tree.set(tr, 1, 10);
    tree.set(tr, 3, 1000);
    tree.set(tr, 4, 10000);
    EXPECT_EQ(tree.all_prod(tr), 11011);
    tree.set_e(tr, 1);
    EXPECT_EQ(tree.all_prod(tr), 11001);
    tree.set_e(tr, 2);
    EXPECT_EQ(tree.all_prod(tr), 11001);
}
