
#include "yosupo/math/primitive_root.hpp"

#include "gtest/gtest.h"

TEST(MathPrimitiveRootTest, SmallestPrimitiveRoot) {
    ASSERT_EQ(yosupo::smallest_primitive_root(998244353), 3);
}
