#include "yosupo/bit.hpp"
#include <numeric>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(BitTest, CeilPow2) {
    ASSERT_EQ(0, ceil_pow2(0));
    ASSERT_EQ(0, ceil_pow2(1));
    ASSERT_EQ(1, ceil_pow2(2));
    ASSERT_EQ(2, ceil_pow2(3));
    ASSERT_EQ(2, ceil_pow2(4));
    ASSERT_EQ(3, ceil_pow2(5));
    ASSERT_EQ(3, ceil_pow2(6));
    ASSERT_EQ(3, ceil_pow2(7));
    ASSERT_EQ(3, ceil_pow2(8));
    ASSERT_EQ(4, ceil_pow2(9));
    ASSERT_EQ(30, ceil_pow2(1 << 30));
    ASSERT_EQ(31, ceil_pow2((1 << 30) + 1));
    ASSERT_EQ(31, ceil_pow2(std::numeric_limits<int>::max()));
}

TEST(BitTest, BSF) {
    ASSERT_EQ(0, bsf(1U));
    ASSERT_EQ(1, bsf(2U));
    ASSERT_EQ(0, bsf(3U));
    ASSERT_EQ(2, bsf(4U));
    ASSERT_EQ(0, bsf(5U));
    ASSERT_EQ(1, bsf(6U));
    ASSERT_EQ(0, bsf(7U));
    ASSERT_EQ(3, bsf(8U));
    ASSERT_EQ(0, bsf(9U));
    ASSERT_EQ(30, bsf(1U << 30));
    ASSERT_EQ(0, bsf((1U << 31) - 1));
    ASSERT_EQ(31, bsf(1U << 31));
    ASSERT_EQ(0, bsf(std::numeric_limits<unsigned int>::max()));
}

TEST(BitTest, BSR) {
    ASSERT_EQ(0, bsr(1U));
    ASSERT_EQ(1, bsr(2U));
    ASSERT_EQ(1, bsr(3U));
    ASSERT_EQ(2, bsr(4U));
    ASSERT_EQ(2, bsr(5U));
    ASSERT_EQ(2, bsr(6U));
    ASSERT_EQ(2, bsr(7U));
    ASSERT_EQ(3, bsr(8U));
    ASSERT_EQ(3, bsr(9U));
    ASSERT_EQ(30, bsr(1U << 30));
    ASSERT_EQ(30, bsr((1U << 31) - 1));
    ASSERT_EQ(31, bsr(1U << 31));
    ASSERT_EQ(31, bsr(std::numeric_limits<unsigned int>::max()));
}

TEST(BitTest, BSRLL) {
    ASSERT_EQ(0, bsr(1ULL));
    ASSERT_EQ(1, bsr(2ULL));
    ASSERT_EQ(1, bsr(3ULL));
    ASSERT_EQ(2, bsr(4ULL));
    ASSERT_EQ(2, bsr(5ULL));
    ASSERT_EQ(2, bsr(6ULL));
    ASSERT_EQ(2, bsr(7ULL));
    ASSERT_EQ(3, bsr(8ULL));
    ASSERT_EQ(3, bsr(9ULL));
    ASSERT_EQ(62, bsr(1ULL << 62));
    ASSERT_EQ(62, bsr((1ULL << 63) - 1));
    ASSERT_EQ(63, bsr(1ULL << 63));
    ASSERT_EQ(63, bsr(std::numeric_limits<unsigned long long>::max()));
}
