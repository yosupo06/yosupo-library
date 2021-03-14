#include "yosupo/math.hpp"

#include <algorithm>
#include <numeric>

#include "gtest/gtest.h"

bool naive_is_prime(int x) {
    if (x <= 1) return false;
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return false;
    }
    return true;
}

TEST(MathTest, Prime) {
    for (int i = 0; i < 10000; i++) {        
        ASSERT_EQ(naive_is_prime(i), yosupo::is_prime(i));
    }
}

TEST(MathTest, GCD) {
    for (int a = -100; a < 100; a++) {
        for (int b = -100; b < 100; b++) {
            ASSERT_EQ(std::gcd(std::abs(a), std::abs(b)), yosupo::gcd(a, b));
        }
    }
}
