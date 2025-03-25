#include "yosupo/modvec.hpp"

#include "gtest/gtest.h"
#include "yosupo/modint.hpp"

using modint = yosupo::ModInt<998244353>;
using modvec = yosupo::ModVec<998244353>;

TEST(ModVecTest, Index) {
    modvec a({1, 2, 3, 4});
    EXPECT_EQ(a[2], modint(3));
    a[1] = 100;
    EXPECT_EQ(a, modvec({1, 100, 3, 4}));
}

TEST(ModVecTest, Equality) {
    modvec a({1, 2, 3, 4});
    modvec b({1, 2, 3, 4});
    modvec c({1, 2, 3, 5});
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(ModVecTest, Add) {
    {
        modvec a({1, 2, 3, 4});
        modvec b({5, 6, 7, 8});
        modvec c({6, 8, 10, 12});
        EXPECT_EQ(a + b, c);
        a += b;
        EXPECT_EQ(a, c);
    }
    {
        modvec a({1, 2, 3, 4});
        modvec b({5, 6, 7});
        modvec c({6, 8, 10, 4});
        EXPECT_EQ(a + b, c);
        EXPECT_EQ(b + a, c);
    }
}

TEST(ModVecTest, Subtract) {
    {
        modvec a({1, 2, 3, 4});
        modvec b({5, 6, 7, 8});
        modvec c({-4, -4, -4, -4});
        EXPECT_EQ(a - b, c);
        a -= b;
        EXPECT_EQ(a, c);
    }
    {
        modvec a({1, 2, 3, 4});
        modvec b({5, 6, 7});
        modvec c({-4, -4, -4, 4});
        EXPECT_EQ(a - b, c);
        modvec d({4, 4, 4, -4});
        EXPECT_EQ(b - a, d);
    }
}

TEST(ModVecTest, Dump) {
    modvec a({1, 2, 3, 4});
    EXPECT_EQ(a.dump(), "[1, 2, 3, 4]");
}
