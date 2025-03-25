
#include "yosupo/math/prime.hpp"

#include <vector>

#include "gtest/gtest.h"

TEST(MathPrimeTest, IsPrime) {
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

TEST(MathPrimeTest, LPFTable) {
    EXPECT_EQ(yosupo::lpf_table(0), std::vector<int>({-1}));
    EXPECT_EQ(yosupo::lpf_table(1), std::vector<int>({-1, -1}));
    EXPECT_EQ(yosupo::lpf_table(10),
              std::vector<int>({-1, -1, 2, 3, 2, 5, 2, 7, 2, 3, 2}));
}

TEST(MathPrimeTest, Primes) {
    EXPECT_EQ(yosupo::primes(0), std::vector<int>());
    EXPECT_EQ(yosupo::primes(1), std::vector<int>());
    EXPECT_EQ(yosupo::primes(20),
              std::vector<int>({2, 3, 5, 7, 11, 13, 17, 19}));
}
