#include "yosupo/container/hashset.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(HashSetTest, Usage) {
    IncrementalHashSet<int> h;

    EXPECT_EQ(h.find(123), h.end());
    h.insert(123);
    EXPECT_NE(h.find(123), h.end());
}
