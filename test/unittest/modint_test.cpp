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
    ASSERT_EQ(3, mint(int32_t(3)).val());
    ASSERT_EQ(3, mint(uint32_t(3)).val());
    ASSERT_EQ(MOD - 3, mint(int64_t(-3)).val());
    ASSERT_EQ(3, mint(uint64_t(3)).val());
}

TEST(ModIntTest, Val) {
    for (int i = 0; i < 100; i++) {
        mint x(i);
        ASSERT_EQ(i, x.val());
    }
}

TEST(ModIntTest, Assign) {
    mint x = 123;
    mint y;
    y = x;
    ASSERT_EQ(123, y.val());
}

TEST(ModIntTest, Add) {
    ASSERT_EQ(mint(12), mint(2) + mint(10));
    ASSERT_EQ(mint(1), mint(MOD - 1000) + mint(1001));
}

TEST(ModIntTest, Sub) {
    ASSERT_EQ(mint(8), mint(10) - mint(2));
    ASSERT_EQ(mint(MOD - 10), mint(10) - mint(20));
}

TEST(ModIntTest, Mul) {
    ASSERT_EQ(mint(20), mint(2) * mint(10));
    ASSERT_EQ(mint(1), mint(2) * mint(MOD / 2 + 1));
}

TEST(ModIntTest, Equal) {
    ASSERT_TRUE(mint(23) == mint(23));
    ASSERT_FALSE(mint(23) == mint(34));
}

TEST(ModIntTest, Pow) {
    ASSERT_EQ(mint(8), mint(2).pow(3));
    ASSERT_EQ(mint(1), mint(0).pow(0));
}

TEST(ModIntTest, Inv) {
    for (int i = 1; i <= 100; i++) {
        ASSERT_EQ(mint(1), mint(i) * mint(i).inv());
    }
}

TEST(ModIntTest, Dump) {
    ASSERT_EQ("123", mint(123).dump());
    ASSERT_EQ("998244352", mint(MOD - 1).dump());
}
