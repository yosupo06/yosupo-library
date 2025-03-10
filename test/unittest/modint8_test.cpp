#include "yosupo/modint8.hpp"
#include "yosupo/types.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>

#include "gtest/gtest.h"

using namespace yosupo;

const u32 MOD = 998244353;
using modint = ModInt<MOD>;
using modint8 = ModInt8<MOD>;

TEST(ModInt8Test, Val) {
    ASSERT_EQ((std::array<u32, 8>({0, 0, 1, 1, 2, 2, 3, 3})),
              modint8(0, 0, 1, 1, 2, 2, 3, 3).val());
    ASSERT_EQ((std::array<u32, 8>({0, 1, 2, 3, 4, 5, 6, 7})),
              modint8(0, 1, 2, 3, 4, 5, 6, 7).val());

    ASSERT_EQ((std::array<u32, 8>({3, 3, 3, 3, 3, 3, 3, 3})),
              modint8::set1(3).val());
}

TEST(ModInt8Test, ToArray) {
    ASSERT_EQ((std::array<modint, 8>({0, 1, 2, 3, 4, 5, 6, 7})),
              modint8(0, 1, 2, 3, 4, 5, 6, 7).to_array());
}

TEST(ModInt8Test, Add) {
    modint8 a(1, 2, 3, 4, 5, 6, 7, 8 + 1000);
    modint8 b(1, 2, 3, 4, 5, 6, 7, 8 + MOD - 1000);

    modint8 expect(2, 4, 6, 8, 10, 12, 14, 16);

    ASSERT_EQ(expect, (a + b));
}

TEST(ModInt8Test, Sub) {
    modint8 a(11, 22, 33, 44, 55, 66, 77, 88);
    modint8 b(1, 2, 3, 4, 5, 6, 7, 8);

    modint8 expect(10, 20, 30, 40, 50, 60, 70, 80);

    ASSERT_EQ(expect, (a - b));
}

TEST(ModInt8Test, Mul) {
    modint8 a(1, 2, 3, 4, 5, 6, 7, 8);
    modint8 b(10, 20, 30, 40, 50, 60, 70, 80);

    modint8 expect(10, 40, 90, 160, 250, 360, 490, 640);

    ASSERT_EQ(expect, (a * b));
}

TEST(ModInt8Test, Equal) {
    modint8 a(1, 2, 3, 4, 5, 6, 7 + MOD, 8);
    modint8 b(1, 2, 3 + MOD, 4, 5 + MOD, 6, 7, 8);
    modint8 c(1, 2, 4, 3, 5, 6, 7, 8);

    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a == c);
}

TEST(ModInt8Test, PermuteVar) {
    modint8 a(0, 10, 20, 30, 40, 50, 60, 70);

    ASSERT_EQ(modint8(60, 30, 20, 50, 0, 10, 40, 70),
              a.permutevar({6, 3, 2, 5, 0, 1, 4, 7}));
}
TEST(ModInt8Test, Rotate) {
    modint8 a(0, 1, 2, 3, 4, 5, 6, 7);
    ASSERT_EQ((std::array<u32, 8>({3, 4, 5, 6, 7, 0, 1, 2})),
              a.rotate(3).val());
}

TEST(ModInt8Test, Blend) {
    modint8 a(1, 2, 3, 4, 5, 6, 7, 8);
    modint8 b(10, 20, 30, 40, 50, 60, 70, 80);

    ASSERT_EQ(modint8(1, 20, 30, 4, 5, 6, 7, 80), blend<0b10000110>(a, b));
}
TEST(ModInt8Test, BlendVar) {
    modint8 a(1, 2, 3, 4, 5, 6, 7, 8);
    modint8 b(10, 20, 30, 40, 50, 60, 70, 80);

    ASSERT_EQ(modint8(10, 2, 30, 40, 50, 6, 70, 80),
              blendvar(a, b, {1, 0, -1u, 1, 1, 0, 100, 1u << 31}));
}

TEST(ModInt8Test, Neg) {
    modint8 a(1, 2, 3, 4, 5, 6, 7, 8);

    ASSERT_EQ(modint8(1, 2, 3, 4, 5, 6, 7, 8), a);

    ASSERT_EQ(modint8(-1, -2, -3, -4, -5, -6, -7, -8), -a);
}

TEST(ModInt8Test, Dump) {
    ASSERT_EQ("[1, 2, 3, 4, 5, 6, 7, 8]",
              modint8(1, 2, 3, 4, 5, 6, 7, 8).dump());
}