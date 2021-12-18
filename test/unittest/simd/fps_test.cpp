#include "yosupo/random.hpp"
#include "yosupo/simd/fps.hpp"

#include <array>
#include <numeric>
#include <random>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(SimdFPSTest, Pre) {
    using mint = static_modint<998244353>;
    for (int n = 1; n < 20; n++) {
        for (int m = 0; m <= n; m++) {
            std::vector<mint> a(n);
            for (int i = 0; i < n; i++) {
                a[i] = uniform((i == 0) ? 1 : 0, 998244352);
            }
            FPS<998244353> f(a);
            auto g = f.pre(m).to_vec();
            ASSERT_EQ(m, g.size());
            a.resize(m);
            ASSERT_EQ(a, g);
        }
    }
}

TEST(SimdFPSTest, Inv) {
    using mint = static_modint<998244353>;
    for (int n = 1; n < 100; n++) {
        std::vector<mint> a(n);
        for (int i = 0; i < n; i++) {
            a[i] = uniform((i == 0) ? 1 : 0, 998244352);
        }
        FPS<998244353> f(a);
        auto g = f.inv(n);
        auto v = (f * g).pre(n).to_vec();

        int m = int(v.size());
        ASSERT_GE(m, 1);
        std::vector<mint> actual(n);
        actual[0] = 1;
        ASSERT_EQ(actual, v);
    }
}
