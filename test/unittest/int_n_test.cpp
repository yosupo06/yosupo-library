#include "yosupo/int_n.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <sstream>

using namespace yosupo;

using BigUInt = UintN<10>;
using BigInt = IntN<10>;

BigUInt bigu0 = BigUInt("123456789123456789123456789");
BigUInt bigu1 = BigUInt("999888777666555444333222111");

TEST(UIntNTest, Extend) {
    UintN<3> a(12345);
    EXPECT_EQ(BigUInt(a), BigUInt("12345"));
}

TEST(UIntNTest, ReadStr) {
    EXPECT_EQ(BigUInt("123"), BigUInt(123));
    EXPECT_EQ(BigUInt("123456789123456789123456789"), bigu0);
    __int128 x = (__int128)(123456789) *
                 ((__int128)1000000000 * 1000000000 + 1000000000 + 1);

    EXPECT_EQ(BigUInt(x), bigu0);
}

TEST(UIntNTest, Neg) {
    BigUInt a(-1), b(1234), c(-1234);
    EXPECT_EQ(-(-a), a);
    EXPECT_EQ(-b, c);
    EXPECT_EQ(-bigu0, BigUInt("-123456789123456789123456789"));
}

TEST(UIntNTest, Add) {
    EXPECT_EQ(bigu0 + bigu1, BigUInt("1123345566790012233456678900"));
    EXPECT_EQ((-bigu0) + bigu1, BigUInt("876431988543098655209765322"));
}
TEST(UIntNTest, AddCarry) {
    BigUInt a(0), b(1);
    EXPECT_EQ(a + b, BigUInt("1"));
    EXPECT_EQ(a + BigUInt(std::numeric_limits<uint64_t>::max()),
              BigUInt("18446744073709551615"));
    EXPECT_EQ(b + BigUInt(std::numeric_limits<uint64_t>::max()),
              BigUInt("18446744073709551616"));
}

TEST(UIntNTest, Sub) {
    EXPECT_EQ(bigu0 - bigu1, -BigUInt("876431988543098655209765322"));
    EXPECT_EQ(bigu1 - bigu0, BigUInt("876431988543098655209765322"));
}
TEST(UIntNTest, Mul) {
    EXPECT_EQ(
        bigu0 * bigu1,
        BigUInt("123443057971290905819138753543557274695709426847861579"));
}

TEST(UIntNTest, DivSmall) {
    EXPECT_EQ(BigUInt(1000) / 10U, BigUInt(100));
    EXPECT_EQ(BigUInt(1099) / 100U, BigUInt(10));
    EXPECT_EQ(BigUInt(1100) / 100U, BigUInt(11));
    EXPECT_EQ(bigu0 / 1U, bigu0);
    EXPECT_EQ(bigu0 / 123U, BigUInt("1003713732711030805881762"));
}
// TEST(UIntNTest, Div) {
//     EXPECT_EQ(BigUInt(1000) / BigUInt(10), BigUInt(100));
//     EXPECT_EQ(BigUInt(1099) / BigUInt(100), BigUInt(10));
//     EXPECT_EQ(BigUInt(1100) / BigUInt(100), BigUInt(11));
//     EXPECT_EQ(bigu0 / bigu1, BigUInt(0));
//     EXPECT_EQ(bigu1 / bigu0, BigUInt(8));
//     EXPECT_EQ(bigu0 / BigUInt(1), bigu0);
// }

TEST(UIntNTest, RemSmall) {
    EXPECT_EQ(BigUInt(1000) % 10U, BigUInt(0));
    EXPECT_EQ(BigUInt(1099) % 100U, BigUInt(99));
    EXPECT_EQ(BigUInt(1100) % 100U, BigUInt(0));
    EXPECT_EQ(bigu0 % 1U, BigUInt(0));
    EXPECT_EQ(bigu0 % 12345U, BigUInt(4329));
}

TEST(UIntNTest, LShift) {
    EXPECT_EQ(BigUInt(1) << 1, BigUInt(2));
    EXPECT_EQ(BigUInt(1) << 100, BigUInt("1267650600228229401496703205376"));
    EXPECT_EQ(BigUInt(1) << 320 << 320, BigUInt(0));
    EXPECT_EQ(BigUInt((uint64_t)(-1)) << 63,
              BigUInt("170141183460469231722463931679029329920"));
    EXPECT_EQ(BigUInt((uint64_t)(-1)) << 64,
              BigUInt("340282366920938463444927863358058659840"));
    EXPECT_EQ(BigUInt((uint64_t)(-1)) << 10 << 54,
              BigUInt("340282366920938463444927863358058659840"));
    EXPECT_EQ(BigUInt((uint64_t)(-1)) << 100,
              BigUInt("23384026197294446689991306723232298912998217482240"));
    EXPECT_EQ(BigUInt((uint64_t)(-1)) << 70 << 30,
              BigUInt("23384026197294446689991306723232298912998217482240"));
}

TEST(UIntNTest, ToBool) {
    EXPECT_TRUE((bool)(BigUInt(1000)));
    EXPECT_FALSE((bool)(BigUInt(0)));
}

TEST(UIntNTest, RShift) {
    EXPECT_EQ(BigUInt(1) >> 0, BigUInt(1));
    EXPECT_EQ(BigUInt(1) >> 1, BigUInt(0));
    EXPECT_EQ(BigUInt(1) << 100 >> 100, BigUInt(1));
    EXPECT_EQ(BigUInt(1) << 639 >> 639, BigUInt(1));
}

TEST(UIntNTest, BitWidth) {
    EXPECT_EQ(BigUInt(0).bit_width(), 0);
    EXPECT_EQ(BigUInt(1).bit_width(), 1);
    EXPECT_EQ((BigUInt(1) << 100).bit_width(), 101);
    EXPECT_EQ((BigUInt(1) << 639).bit_width(), 640);
}

TEST(UIntNTest, Ostream) {
    {
        BigUInt a("123456789123456789123456789");
        std::stringstream ss;
        ss << a;
        EXPECT_EQ(ss.str(), "123456789123456789123456789");
    }
    {
        BigUInt zero(0);
        std::stringstream ss;
        ss << zero;
        EXPECT_EQ(ss.str(), "0");
    }
    {
        BigUInt large = BigUInt(1) << 100;
        std::stringstream ss;
        ss << large;
        EXPECT_EQ(ss.str(), "1267650600228229401496703205376");
    }
}

TEST(IntNTest, IsNeg) {
    EXPECT_TRUE(BigInt(-123).is_negative());
    EXPECT_FALSE(BigInt(0).is_negative());
    EXPECT_FALSE(BigInt(234).is_negative());
}

TEST(IntNTest, Comp) {
    BigInt a(-123), b(-12), c(23), d(234);
    EXPECT_LT(a, b);
    EXPECT_LT(b, c);
    EXPECT_LT(c, d);

    EXPECT_GT(b, a);
    EXPECT_GT(c, b);
    EXPECT_GT(d, c);
}

TEST(IntNTest, Comp1) {
    IntN<1> a(-123), b(-12), c(23), d(234);
    EXPECT_LT(a, b);
    EXPECT_LT(b, c);
    EXPECT_LT(c, d);

    EXPECT_GT(b, a);
    EXPECT_GT(c, b);
    EXPECT_GT(d, c);
}

TEST(IntNTest, Abs) {
    BigInt a(-123), b(123);
    EXPECT_EQ(a.abs(), b);
    EXPECT_EQ(b.abs(), b);
}
TEST(IntNTest, Mul) {
    EXPECT_EQ(BigInt(123) * BigInt(234), BigInt(28782));
    EXPECT_EQ(BigInt(-123) * BigInt(234), BigInt(-28782));
    EXPECT_EQ(BigInt(123) * BigInt(-234), BigInt(-28782));
    EXPECT_EQ(BigInt(-123) * BigInt(-234), BigInt(28782));
}
