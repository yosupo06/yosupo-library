#include "yosupo/convolution.hpp"

#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/modint.hpp"

using namespace yosupo;
using mint = ModInt998244353;

TEST(ConvolutionTest, Small) {
    std::vector<mint> a = {1, 2, 3, 4};
    std::vector<mint> b = {5, 6, 7, 8, 9};

    auto c = convolution(std::move(a), std::move(b));

    EXPECT_EQ(c, std::vector<mint>({5, 16, 34, 60, 70, 70, 59, 36}));
}

TEST(ConvolutionTest, Large) {
    std::vector<mint> a(111);
    std::vector<mint> b(222);

    for (int i = 0; i < 111; i++) {
        a[i] = i;
    }
    for (int i = 0; i < 222; i++) {
        b[i] = i;
    }

    std::vector<mint> c_expect(111 + 222 - 1);
    for (int i = 0; i < 111; i++) {
        for (int j = 0; j < 222; j++) {
            c_expect[i + j] += i * j;
        }
    }

    auto c_actual = convolution(std::move(a), std::move(b));

    EXPECT_EQ(c_actual, c_expect);
}
