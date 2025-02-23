#include "yosupo/bit.hpp"

#include "gtest/gtest.h"

TEST(BitTest, CountRZero) {
    ASSERT_EQ(yosupo::countr_zero((unsigned int)(12)), 2);
    ASSERT_EQ(yosupo::countr_zero((unsigned __int128)(12)), 2);
}
