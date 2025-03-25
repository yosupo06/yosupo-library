#include "yosupo/sparsetable.hpp"

#include <vector>

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"

TEST(SparseTable, Usage) {
    yosupo::SparseTable<yosupo::Max<int>> sp(std::vector<int>{1, 3, 2});
    ASSERT_EQ(sp.query(0, 3), 3);
}
