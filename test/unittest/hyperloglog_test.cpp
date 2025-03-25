#include "yosupo/hyperloglog.hpp"

#include "gtest/gtest.h"

#include "yosupo/random.hpp"

#include "yosupo/types.hpp"

using namespace yosupo;

TEST(HyperLogLogTest, Random100) {
    HyperLogLog<8, 0> hll;
    for (int i = 0; i < 100; i++) {
        hll.add(uniform<u32>(0, -1));
    }
    int est = hll.estimate();

    ASSERT_LE(50, est);
    ASSERT_LE(est, 200);
}

TEST(HyperLogLogTest, Linear) {
    for (int n = 0; n <= 100; n++) {
        HyperLogLog<4, 100> hll;
        for (int i = 0; i < n; i++) {
            hll.add(i);
        }
        int est = hll.estimate();

        ASSERT_EQ(est, n);
    }
}
