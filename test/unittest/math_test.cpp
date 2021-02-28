#include "yosupo/math.hpp"

#include <algorithm>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

bool naive_is_prime(int x) {
    if (x <= 1) return false;
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return false;
    }
    return true;
}

TEST(MathTest, Prime) {
    for (int i = 0; i < 10000; i++) {        
        ASSERT_EQ(naive_is_prime(i), is_prime(i));
    }
}
