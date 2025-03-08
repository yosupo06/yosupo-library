#include "yosupo/comb.hpp"
#include "yosupo/modint.hpp"

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;
using mint = ModInt998244353;

TEST(CombTest, Fact) {
    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(mint(1), (fact<mint>(i) * inv_fact<mint>(i)));
    }
}

TEST(CombTest, Comb) {
    for (int n = 0; n < 100; n++) {
        for (int k = 0; k <= n; k++) {
            ASSERT_EQ(
                (fact<mint>(n) * inv_fact<mint>(k) * inv_fact<mint>(n - k)),
                comb<mint>(n, k));
        }
    }

    ASSERT_EQ(mint(0), comb<mint>(3, 10));
    ASSERT_EQ(mint(1), comb<mint>(0, 0));
    ASSERT_EQ(mint(0), comb<mint>(0, 10));
}

TEST(CombTest, NegComb) {
    for (int n = -100; n < 100; n++) {
        for (int k = 0; k <= n; k++) {
            mint expect = inv_fact<mint>(k);
            for (int i = 0; i < k; i++) {
                expect *= mint(n - i);
            }

            ASSERT_EQ(expect, comb<mint>(n, k));
        }
    }
}

TEST(CombTest, Inv) {
    for (int i = 1; i < 100; i++) {
        ASSERT_EQ(mint(1), (mint(i) * inv<mint>(i)));
    }
}
