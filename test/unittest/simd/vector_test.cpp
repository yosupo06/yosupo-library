#include "yosupo/random.hpp"
#include "yosupo/simd/vector.hpp"

#include <numeric>
#include <random>
#include <vector>
#include <array>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(SimdVectorTest, UsageIntx8) {
    {
        // plus
        intx8 a = {1, 2, 3, 4, 5, 6, 7, 8};
        intx8 b = {10, 20, 30, 40, 50, 60, 70, 80};

        auto expect = std::array<int, 8>({11, 22, 33, 44, 55, 66, 77, 88});
        ASSERT_EQ(expect, (a + b).to_array());
    }
    {
        // at, set
        intx8 a = {10, 20, 30, 40, 50, 60, 70, 80};
        ASSERT_EQ(a.at(3), 40);
        a.set(2, 90);

        auto expect = std::array<int, 8>({10, 20, 90, 40, 50, 60, 70, 80});
        ASSERT_EQ(expect, a.to_array());
    }
    {
        // all_zero
        intx8 a = {10, 20, 30, 40, 50, 60, 70, 80};
        intx8 b = {10, 20, 30, 40, 50, 60, 70, 80};
        intx8 c = {10, 22, 30, 40, 50, 60, 70, 80};
        ASSERT_TRUE((a - b).test_all_zero());
        ASSERT_FALSE((a - c).test_all_zero());
    }
    {
        // clear
        intx8 a = {10, 20, 30, 40, 50, 60, 70, 80};
        a.clear(0b01001000);

        auto expect = std::array<int, 8>({10, 20, 30, 0, 50, 60, 0, 80});
        ASSERT_EQ(expect, a.to_array());
    }
}

TEST(SimdVectorTest, Usagellx4) {
    llx4 a = {1, 2, 3, 4};
    llx4 b = {10, 20, 30, 40};

    auto expect = std::array<ll, 4>({11, 22, 33, 44});

    ASSERT_EQ(expect, (a + b).to_array());
}
