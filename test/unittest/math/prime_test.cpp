
#include "yosupo/math/prime.hpp"

#include "gtest/gtest.h"

TEST(MathPrimeTest, Prime) {
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