#include "yosupo/numeric.hpp"

#include <utility>

#include "gtest/gtest.h"

TEST(NumericTest, Abs) {
    EXPECT_EQ(yosupo::abs(-123), 123);
    EXPECT_EQ(yosupo::abs((__int128)-123), 123);
}

TEST(NumericTest, Gcd) {
    __int128 x = 1;
    x <<= 100;
    EXPECT_EQ(yosupo::gcd(x, x), x);
}

TEST(NumericTest, GcdStress) {
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

TEST(NumericTest, Sgn) {
    EXPECT_EQ(yosupo::sgn(-123), -1);
    EXPECT_EQ(yosupo::sgn(0), 0);
    EXPECT_EQ(yosupo::sgn(123), 1);
    EXPECT_EQ(yosupo::sgn((__int128)-123), -1);
    EXPECT_EQ(yosupo::sgn((__int128)0), 0);
    EXPECT_EQ(yosupo::sgn((__int128)123), 1);
}
