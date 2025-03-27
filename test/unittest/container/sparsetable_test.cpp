#include "yosupo/container/sparsetable.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"

TEST(SparseTable, Usage) {
    yosupo::SparseTable<yosupo::Max<int>> sp(std::vector<int>{1, 3, 2});
    ASSERT_EQ(sp.query(0, 3), 3);
}

TEST(SparseTable, Lambda) {
    yosupo::SparseTable sp(
        std::vector<int>{1, 3, 2},
        yosupo::Monoid(12345, [](int a, int b) { return std::min(a, b); }));
    ASSERT_EQ(sp.query(0, 3), 1);
    ASSERT_EQ(sp.query(0, 0), 12345);
}
