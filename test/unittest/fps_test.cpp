#include "yosupo/fps.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/random.hpp"

#include <numeric>

#include "gtest/gtest.h"

#include "../utils/random.hpp"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(FPSTest, PowTest) {
    for (int k = 1; k < 10; k++) {
        std::vector<modint998244353> v(k);
        for (int i = 0; i < k; i++) {
            v[i] = uniform(1, 998244352);
        }
        FPS<modint998244353> p(v);
        for (int n = 1; n < 10; n++) {
            auto q_expect = FPS<modint998244353>({1});
            for (int i = 0; i < n; i++) {
                q_expect *= p;
            }
            auto q_actual = p.pow(n);
            ASSERT_EQ(q_expect, q_actual);
        }
    }
}
