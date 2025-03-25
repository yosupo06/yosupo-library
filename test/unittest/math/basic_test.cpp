
#include "yosupo/math/basic.hpp"

#include "gtest/gtest.h"

TEST(MathBasicTest, ISqrtTest) {
    EXPECT_EQ(yosupo::isqrt(99), 9);
    EXPECT_EQ(yosupo::isqrt(100), 10);
    EXPECT_EQ(yosupo::isqrt(101), 10);
}

TEST(MathBasicTest, IRootTest) {
    EXPECT_EQ(yosupo::iroot(99, 2), 9);
    EXPECT_EQ(yosupo::iroot(100, 2), 10);
    EXPECT_EQ(yosupo::iroot(101, 2), 10);

    EXPECT_EQ(yosupo::iroot(999, 3), 9);
    EXPECT_EQ(yosupo::iroot(1000, 3), 10);
    EXPECT_EQ(yosupo::iroot(1001, 3), 10);
}

TEST(MathBasicTest, InvU32) {
    for (unsigned int i = 1; i < 100u; i += 2) {
        unsigned int j = yosupo::inv_u32(i);
        ASSERT_EQ((i * j), 1u);
    }
}
