#include "yosupo/hashmap.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(HashMapTest, Iterator) {
    IncrementalHashMap<int, int> h;
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
    std::sort(actual.begin(), actual.end());
    ASSERT_EQ(expect, actual);

    h[4] += 3;

    ASSERT_EQ(3, h[4]);

    h[2] += 5;

    ASSERT_EQ(5, h[2]);

    expect = {{2, 5}, {3, 1}, {4, 3}, {6, 3}};
    actual = std::vector<std::pair<int, int>>(h.begin(), h.end());
    std::sort(actual.begin(), actual.end());
    ASSERT_EQ(expect, actual);
}

TEST(HashMapTest, Assign) {
    IncrementalHashMap<int, int> h;

    for (int i = 0; i < 100; i++) {
        h[i] = i;
    }

    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(i, h[i]);
    }

    auto h2 = h;

    for (int i = 0; i < 100; i++) {
        h2[i] = 2 * i;
    }

    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(i, h[i]);
        ASSERT_EQ(2 * i, h2[i]);
    }
}

TEST(HashMapTest, Rewrite) {
    IncrementalHashMap<int, int> h;

    h[3] = 100;

    auto it = h.find(3);

    it->second = 101;

    ASSERT_EQ(101, h[3]);

    (*it).second = 102;

    ASSERT_EQ(102, h[3]);
}
