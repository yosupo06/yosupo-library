#include "yosupo/math.hpp"
#include "gtest/gtest.h"

TEST(MathTest, Abs) {
    EXPECT_EQ(yosupo::abs(-123), 123);
    EXPECT_EQ(yosupo::abs((__int128)-123), 123);
}

TEST(MathTest, Gcd) {
    __int128 x = 1;
    x <<= 100;
    EXPECT_EQ(yosupo::gcd(x, x), x);
}

TEST(MathTest, GcdStress) {
    auto gcd = [&](__int128 a, __int128 b) {
        if (a < 0) a = -a;
        if (b < 0) b = -b;
        while (b) {
            a %= b;
            std::swap(a, b);
        }
        return a;
    };

    for (int a = -100; a <= 100; a++) {
        for (int b = -100; b <= 100; b++) {
            ASSERT_EQ(yosupo::gcd(a, b), gcd(a, b));
        }
    }
}

TEST(MathTest, Sgn) {
    EXPECT_EQ(yosupo::sgn(-123), -1);
    EXPECT_EQ(yosupo::sgn(0), 0);
    EXPECT_EQ(yosupo::sgn(123), 1);
    EXPECT_EQ(yosupo::sgn((__int128)-123), -1);
    EXPECT_EQ(yosupo::sgn((__int128)0), 0);
    EXPECT_EQ(yosupo::sgn((__int128)123), 1);
}

TEST(MathTest, FloorDiv) {
    EXPECT_EQ(yosupo::floor_div(11, 5), 2);
    EXPECT_EQ(yosupo::floor_div(10, 5), 2);
    EXPECT_EQ(yosupo::floor_div(9, 5), 1);
    EXPECT_EQ(yosupo::floor_div(-9, 5), -2);
    EXPECT_EQ(yosupo::floor_div(-10, 5), -2);
    EXPECT_EQ(yosupo::floor_div(-11, 5), -3);
}

TEST(MathTest, CeilDiv) {
    EXPECT_EQ(yosupo::ceil_div(11, 5), 3);
    EXPECT_EQ(yosupo::ceil_div(10, 5), 2);
    EXPECT_EQ(yosupo::ceil_div(9, 5), 2);
    EXPECT_EQ(yosupo::ceil_div(-9, 5), -1);
    EXPECT_EQ(yosupo::ceil_div(-10, 5), -2);
    EXPECT_EQ(yosupo::ceil_div(-11, 5), -2);
}

TEST(MathTest, Prime) {
    auto is_prime = [&](int x) {
        if (x <= 1) return false;
        for (int i = 2; i * i <= x; i++) {
            if (x % i == 0) return false;
        }
        return true;
    };
    for (int i = 0; i < 10000; i++) {
        ASSERT_EQ(yosupo::is_prime(i), is_prime(i));
    }
}

TEST(MathTest, InvU32) {
    for (unsigned int i = 1; i < 100u; i += 2) {
        unsigned int j = yosupo::inv_u32(i);
        ASSERT_EQ((i * j), 1u);
    }
}

TEST(MathTest, SmallestPrimitiveRoot) {
    ASSERT_EQ(yosupo::smallest_primitive_root(998244353), 3);
}
