#include "yosupo/modint.hpp"
#include "yosupo/types.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>

#include "gtest/gtest.h"

using namespace yosupo;
const int MOD = 998244353;
using mint = ModInt<MOD>;

TEST(ModIntTest, Constructor) {
    EXPECT_EQ(3, mint(int32_t(3)).val());
    EXPECT_EQ(3, mint(uint32_t(3)).val());
    EXPECT_EQ(MOD - 3, mint(int64_t(-3)).val());
    EXPECT_EQ(3, mint(uint64_t(3)).val());
}

TEST(ModIntTest, Val) {
    for (int i = 0; i < 100; i++) {
        mint x(i);
        EXPECT_EQ(i, x.val());
    }
}

TEST(ModIntTest, Assign) {
    mint x = 123;
    mint y;
    y = x;
    EXPECT_EQ(123, y.val());
}

TEST(ModIntTest, Add) {
    EXPECT_EQ(mint(12), mint(2) + mint(10));
    EXPECT_EQ(mint(1), mint(MOD - 1000) + mint(1001));
}

TEST(ModIntTest, Sub) {
    EXPECT_EQ(mint(8), mint(10) - mint(2));
    EXPECT_EQ(mint(MOD - 10), mint(10) - mint(20));
}

TEST(ModIntTest, Mul) {
    EXPECT_EQ(mint(20), mint(2) * mint(10));
    EXPECT_EQ(mint(1), mint(2) * mint(MOD / 2 + 1));
}

TEST(ModIntTest, Div) {
    EXPECT_EQ(mint(123) / mint(234) * mint(234), mint(123));
    EXPECT_EQ(mint(123) / mint(234) / mint(345), mint(123) / mint(234 * 345));
}

TEST(ModIntTest, Equal) {
    ASSERT_TRUE(mint(23) == mint(23));
    ASSERT_FALSE(mint(23) == mint(34));
}

TEST(ModIntTest, Pow) {
    EXPECT_EQ(mint(8), mint(2).pow(3));
    EXPECT_EQ(mint(1), mint(0).pow(0));
}

TEST(ModIntTest, Inv) {
    for (int i = 1; i <= 100; i++) {
        EXPECT_EQ(mint(1), mint(i) * mint(i).inv());
    }
}

TEST(ModIntTest, Dump) {
    EXPECT_EQ("123", mint(123).dump());
    EXPECT_EQ("998244352", mint(MOD - 1).dump());
}
