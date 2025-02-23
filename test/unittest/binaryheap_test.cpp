#include "yosupo/binaryheap.hpp"
#include "yosupo/random.hpp"

#include <numeric>
#include <queue>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(BinaryHeapTest, Ssize) {
    MeldableBinaryHeapManager<int> manager;

    auto h = manager.build({1, 2, 3});

    ASSERT_EQ(3, h.size());
    ASSERT_EQ(3, std::ssize(h));
}

TEST(BinaryHeapTest, Meld) {
    MeldableBinaryHeapManager<int> manager;

    auto h0 = manager.build({1, 2, 3});
    auto h1 = manager.build({4, 5});

    manager.meld(h0, h1);

    ASSERT_EQ(manager.top(h0), 5);
}

TEST(BinaryHeapTest, StressPushPop) {
    for (int ph = 0; ph < 10000; ph++) {
        MeldableBinaryHeapManager<int> manager;

        int n = uniform(1, 10);
        std::vector<int> v(n);
        for (int i = 0; i < n; i++) {
            v[i] = uniform(1, 10);
        }

        auto h0 = manager.build();
        std::priority_queue<int> h1;
        for (auto x : v) {
            manager.push(h0, x);
            h1.push(x);

            ASSERT_EQ(manager.top(h0), h1.top());
        }

        for (int i = 0; i < n - 1; i++) {
            manager.pop(h0);
            h1.pop();

            ASSERT_EQ(manager.top(h0), h1.top());
        }
    }
}

TEST(BinaryHeapTest, PopPush) {
    MeldableBinaryHeapManager<int> manager;

    auto h = manager.build({1, 2, 3});

    ASSERT_EQ(manager.top(h), 3);

    manager.pop_push(h, 0);
    ASSERT_EQ(manager.top(h), 2);
    manager.pop(h);
    ASSERT_EQ(manager.top(h), 1);
    manager.pop(h);
    ASSERT_EQ(manager.top(h), 0);
    manager.pop(h);
    ASSERT_TRUE(h.empty());
}
