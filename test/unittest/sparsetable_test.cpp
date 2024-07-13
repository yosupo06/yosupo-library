#include "yosupo/sparsetable.hpp"

#include "gtest/gtest.h"

struct Max {
    using S = int;
    S op(S a, S b) { return std::max(a, b); }
    S e() { return S(0); }
};

TEST(SparseTable, Usage) {
    yosupo::SparseTable<Max> sp(std::vector<int>{1, 3, 2});
    ASSERT_EQ(sp.query(0, 3), 3);
}
