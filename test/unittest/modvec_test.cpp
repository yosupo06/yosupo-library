#include "yosupo/modvec.hpp"

#include "gtest/gtest.h"

using modint = yosupo::ModInt<998244353>;
using modvec = yosupo::ModVec<998244353>;

TEST(ModVecTest, Index) {
    modvec a({1, 2, 3, 4});
    EXPECT_EQ(a[2], modint(3));
    a[1] = 100;
    EXPECT_EQ(a, modvec({1, 100, 3, 4}));
}
