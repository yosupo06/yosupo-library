#include "yosupo/container/splaytree.hpp"

#include <functional>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"

struct RangeAddMax {
    using S = int;
    using F = int;

    yosupo::Max<S> monoid = yosupo::Max<S>(0);
    yosupo::Monoid<F, std::plus<F>> act = yosupo::Monoid<F, std::plus<F>>(0);

    S mapping(F l, S r) { return l + r; }
};

TEST(SplayTreeTest, ToVec) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    EXPECT_EQ(std::vector<int>({1, 2, 3, 4, 5}), tree.to_vec(tr));
    auto tr2 = tree.build();
    EXPECT_EQ(std::vector<int>({}), tree.to_vec(tr2));
}

TEST(SplayTreeTest, Size) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    EXPECT_EQ(5, tree.size(tr));
    auto tr2 = tree.build();
    EXPECT_EQ(0, tree.size(tr2));
}

TEST(SplayTreeTest, Get) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    EXPECT_EQ(1, tree.get(tr, 0));
    EXPECT_EQ(2, tree.get(tr, 1));
    EXPECT_EQ(3, tree.get(tr, 2));
    EXPECT_EQ(4, tree.get(tr, 3));
    EXPECT_EQ(5, tree.get(tr, 4));
}

TEST(SplayTreeTest, Set) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    tree.set(tr, 0, 10);
    tree.set(tr, 1, 20);
    tree.set(tr, 4, 50);

    ASSERT_EQ(std::vector<int>({10, 20, 3, 4, 50}), tree.to_vec(tr));
}

TEST(SplayTreeTest, Insert) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    tree.insert(tr, 0, 10);
    tree.insert(tr, 1, 20);
    tree.insert(tr, 4, 50);

    ASSERT_EQ(std::vector<int>({10, 20, 1, 2, 50, 3, 4, 5}), tree.to_vec(tr));
}

TEST(SplayTreeTest, AllApply) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    tree.all_apply(tr, 10);
    EXPECT_EQ(std::vector<int>({11, 12, 13, 14, 15}), tree.to_vec(tr));
}

TEST(SplayTreeTest, Apply) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    tree.apply(tr, 1, 4, 10);
    EXPECT_EQ(std::vector<int>({1, 12, 13, 14, 5}), tree.to_vec(tr));
}

TEST(SplayTreeTest, Split) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr1 = tree.build({1, 2, 3, 4, 5});
    auto tr2 = tree.split(tr1, 4);

    ASSERT_EQ(std::vector<int>({1, 2, 3, 4}), tree.to_vec(tr1));
    ASSERT_EQ(std::vector<int>({5}), tree.to_vec(tr2));

    ASSERT_EQ(std::vector<int>({1, 2, 3, 4}), tree.to_vec(tr1));
    auto tr3 = tree.split(tr1, 0);
    ASSERT_EQ(std::vector<int>({}), tree.to_vec(tr1));
    ASSERT_EQ(std::vector<int>({1, 2, 3, 4}), tree.to_vec(tr3));
    auto tr4 = tree.split(tr3, 4);
    ASSERT_EQ(std::vector<int>({1, 2, 3, 4}), tree.to_vec(tr3));
    ASSERT_EQ(std::vector<int>({}), tree.to_vec(tr4));
}

TEST(SplayTreeTest, Merge) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr1 = tree.build({1, 2, 3, 4, 5});
    auto tr2 = tree.build({6, 7, 8});

    auto tr = tree.merge(std::move(tr1), std::move(tr2));

    ASSERT_EQ(std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8}), tree.to_vec(tr));
}

TEST(SplayTreeTest, MaxRight) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});

    ASSERT_EQ(0, tree.max_right(tr, [&](int x) { return x <= 0; }));
    ASSERT_EQ(1, tree.max_right(tr, [&](int x) { return x <= 1; }));
    ASSERT_EQ(2, tree.max_right(tr, [&](int x) { return x <= 2; }));
    ASSERT_EQ(3, tree.max_right(tr, [&](int x) { return x <= 3; }));
    ASSERT_EQ(4, tree.max_right(tr, [&](int x) { return x <= 4; }));
    ASSERT_EQ(5, tree.max_right(tr, [&](int x) { return x <= 5; }));

    auto tr2 = tree.build(RangeAddMax::S(10));

    ASSERT_EQ(0, tree.max_right(tr2, [&](int x) { return x <= 9; }));
    ASSERT_EQ(1, tree.max_right(tr2, [&](int x) { return x <= 10; }));

    auto tr3 = tree.build();

    ASSERT_EQ(0, tree.max_right(tr3, [&](int x) { return x <= 9; }));
}

TEST(SplayTreeTest, Lambda) {
    yosupo::SplayTree tree((yosupo::ActedMonoid(
        yosupo::Max<int>(), yosupo::Monoid<int, std::plus<int>>(0),
        [](int a, int b) { return a + b; })));
    auto tr = tree.build({1, 2, 3, 4, 5});
    EXPECT_EQ(tree.all_prod(tr), 5);
    tree.all_apply(tr, 10);
    EXPECT_EQ(tree.all_prod(tr), 15);
}

TEST(SplayTreeTest, NoLazy) {
    yosupo::SplayTree tree((yosupo::ActedMonoid(yosupo::Max<int>())));
    auto tr = tree.build({1, 2, 3, 4, 5});
    EXPECT_EQ(tree.all_prod(tr), 5);
    tree.set(tr, 1, 10);
    EXPECT_EQ(tree.all_prod(tr), 10);
}
