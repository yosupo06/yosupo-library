#include <numeric>
#include "yosupo/internal_bit.hpp"

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(BitTest, BSR) {
    ASSERT_EQ(0, internal::bsr(1U));
    ASSERT_EQ(1, internal::bsr(2U));
    ASSERT_EQ(1, internal::bsr(3U));
    ASSERT_EQ(2, internal::bsr(4U));
    ASSERT_EQ(2, internal::bsr(5U));
    ASSERT_EQ(2, internal::bsr(6U));
    ASSERT_EQ(2, internal::bsr(7U));
    ASSERT_EQ(3, internal::bsr(8U));
    ASSERT_EQ(3, internal::bsr(9U));
    ASSERT_EQ(30, internal::bsr(1U << 30));
    ASSERT_EQ(30, internal::bsr((1U << 31) - 1));
    ASSERT_EQ(31, internal::bsr(1U << 31));
    ASSERT_EQ(31, internal::bsr(std::numeric_limits<unsigned int>::max()));
}

TEST(BitTest, BSRLL) {
    ASSERT_EQ(0, internal::bsr(1ULL));
    ASSERT_EQ(1, internal::bsr(2ULL));
    ASSERT_EQ(1, internal::bsr(3ULL));
    ASSERT_EQ(2, internal::bsr(4ULL));
    ASSERT_EQ(2, internal::bsr(5ULL));
    ASSERT_EQ(2, internal::bsr(6ULL));
    ASSERT_EQ(2, internal::bsr(7ULL));
    ASSERT_EQ(3, internal::bsr(8ULL));
    ASSERT_EQ(3, internal::bsr(9ULL));
    ASSERT_EQ(62, internal::bsr(1ULL << 62));
    ASSERT_EQ(62, internal::bsr((1ULL << 63) - 1));
    ASSERT_EQ(63, internal::bsr(1ULL << 63));
    ASSERT_EQ(63,
              internal::bsr(std::numeric_limits<unsigned long long>::max()));
}
