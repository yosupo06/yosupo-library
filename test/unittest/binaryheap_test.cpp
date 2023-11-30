#include "yosupo/binaryheap.hpp"
#include "yosupo/random.hpp"

#include <numeric>
#include <queue>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

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
