#include "yosupo/modvec.hpp"

#include "gtest/gtest.h"
#include "yosupo/modint.hpp"

using namespace yosupo;

using modint = ModInt998244353;
using modvec = ModVec998244353;

TEST(ModVecTest, Constructor) {
    {
        modvec a;
        EXPECT_EQ(a.size(), 0);
    }
    {
        modvec a(5);
        modvec b({0, 0, 0, 0, 0});
        EXPECT_EQ(a, b);
    }
    {
        modvec a({1, 2, 3});
        EXPECT_EQ(a.size(), 3);
        EXPECT_EQ(a[0], modint(1));
        EXPECT_EQ(a[1], modint(2));
        EXPECT_EQ(a[2], modint(3));
    }
    {
        modvec a = {2};
        EXPECT_EQ(a.size(), 1);
        EXPECT_EQ(a[0], modint(2));
    }
    {
        modvec a({});
        EXPECT_EQ(a.size(), 0);
    }
}

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

TEST(ModVecTest, Multiply) {
    modvec a({1, 2, 3});
    modvec b({5, 6, 7, 8});
    modvec c({5, 16, 34, 40, 37, 24});
    EXPECT_EQ(a * b, c);
    a *= b;
    EXPECT_EQ(a, c);
}

TEST(ModVecTest, Dump) {
    modvec a({1, 2, 3, 4});
    EXPECT_EQ(a.dump(), "[1, 2, 3, 4]");
}

TEST(ModVecTest, Prod) {
    modvec a({1, 1});
    modvec b({1, 2});
    modvec c({1, 3});
    modvec d({1, 6, 11, 6});
    EXPECT_EQ(modvec::prod({a, b, c}), d);
}
