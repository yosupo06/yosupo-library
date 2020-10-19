#include "yosupo/hashmap.hpp"

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(HashMapTest, Iterator) {
    HashMap<int, int> h;
    h[3] = 1;
    h[6] = 2;

    std::vector<std::pair<int, int>> expect = {{3, 1}, {6, 2}};

    auto actual = std::vector<std::pair<int, int>>(h.begin(), h.end());
    sort(actual.begin(), actual.end());

    ASSERT_EQ(expect, actual);
    ASSERT_EQ(2, h[6]);

    h[6] = 3;
    expect = {{3, 1}, {6, 3}};
    actual = std::vector<std::pair<int, int>>(h.begin(), h.end());
    sort(actual.begin(), actual.end());
    ASSERT_EQ(expect, actual);
}
