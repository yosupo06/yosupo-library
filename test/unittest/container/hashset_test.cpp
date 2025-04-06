#include "yosupo/container/hashset.hpp"

#include <algorithm>
#include <set>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/random.hpp"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(HashSetTest, Usage) {
    IncrementalHashSet<int> h;

    EXPECT_EQ(h.size(), 0);
    EXPECT_EQ(h.find(123), h.end());
    h.insert(123);
    EXPECT_EQ(h.size(), 1);
    EXPECT_NE(h.find(123), h.end());
    h.insert(123);  // Insert duplicate
    EXPECT_EQ(h.size(), 1);
    EXPECT_NE(h.find(123), h.end());
    h.insert(456);
    EXPECT_EQ(h.size(), 2);
    EXPECT_NE(h.find(456), h.end());
}

TEST(HashSetTest, CompareWithSet) {
    IncrementalHashSet<int> h;
    std::set<int> s;
    // Use global random generator

    for (int i = 0; i < 1000; ++i) {
        int val = uniform(0, 999);
        if (uniform_bool()) {
            h.insert(val);
            s.insert(val);
        } else {
            EXPECT_EQ(h.find(val) != h.end(), s.count(val) > 0);
        }
        EXPECT_EQ(h.size(), s.size());
    }

    std::vector<int> h_elements;
    for (int x : h) {
        h_elements.push_back(x);
    }
    std::vector<int> s_elements;
    for (int x : s) {
        s_elements.push_back(x);
    }
    std::ranges::sort(h_elements);
    EXPECT_EQ(h_elements, s_elements);
}
