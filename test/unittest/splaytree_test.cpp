#include "yosupo/splaytree.hpp"

#include <algorithm>
#include <iostream>

#include "gtest/gtest.h"

struct RangeAddMax {
    using S = int;
    using F = int;
    S op(S a, S b) { return std::max(a, b); }
    S e() { return 0; }
    S rev(S a) { return a; }

    S mapping(F l, S r) {
        return l + r;
    }
    F composition(F l, F r) {
        return l + r;
    }
    F id() { return 0; }
};

TEST(SplayTreeTest, Build) {
    yosupo::SplayTree tree((RangeAddMax()));
    auto tr = tree.build({1, 2, 3, 4, 5});
    ASSERT_EQ(std::vector<int>({1, 2, 3, 4, 5}), tree.to_vec(tr));
}
