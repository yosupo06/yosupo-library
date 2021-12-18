#include "yosupo/random.hpp"
#include "yosupo/simd/modint.hpp"
#include "yosupo/simd/convolution.hpp"

#include <array>
#include <numeric>
#include <random>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

template <class mintx8>
std::vector<mintx8> convolution_naive(std::vector<mintx8> a,
                                      std::vector<mintx8> b) {
    int n = int(a.size()), m = int(b.size());
    std::vector<mintx8> c(n + m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    int ij = (i + j) + (k + l) / 8;
                    c[ij].set((k + l) % 8, c[ij].at((k + l) % 8) + a[i].at(k) * b[j].at(l));
                }
            }
        }
    }

    return c;
}

TEST(SimdConvolutionTest, Stress) {
    using mintx8 = modintx8<998244353>;
    for (int n = 1; n < 20; n++) {
        for (int m = 1; m < 20; m++) {
            std::vector<mintx8> a(n), b(m);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < 8; j++) {
                    a[i].set(j, 1); //uniform(0, 998244352));
                }
            }
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < 8; j++) {
                    b[i].set(j, 1); //uniform(0, 998244352));
                }
            }

            auto expect = convolution_naive(a, b);
            auto actual = convolution(a, b);

            ASSERT_EQ(expect.size(), actual.size());
            int k = int(expect.size());
            for (int i = 0; i < k; i++) {
                ASSERT_EQ(expect[i].to_array(), actual[i].to_array());
            }
        }
    }
}

