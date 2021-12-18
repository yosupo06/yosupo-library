#include "yosupo/random.hpp"
#include "yosupo/simd/modint.hpp"

#include <array>
#include <numeric>
#include <random>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(SimdModintTest, Usage) {
    using mint = static_modint<998244353>;
    using mintx8 = modintx8<998244353>;
    {
        // plus
        mintx8 a = {1, 2, 3, 4, 5, 6, 7, 8};
        mintx8 b = {10, 20, 30, 40, 50, 60, 70, 80};

        auto expect = std::array<mint, 8>({11, 22, 33, 44, 55, 66, 77, 88});

        ASSERT_EQ(expect, (a + b).to_array());
    }
    {
        // neg
        mintx8 a = {1, 2, 3, 4, 5, 6, 7, 8};

        auto expect = std::array<mint, 8>({1, -2, 3, 4, 5, 6, 7, -8});

        ASSERT_EQ(expect, a.neg<0b10000010>().to_array());
    }
    {
        // at, set
        mintx8 a = {1, 2, 3, 4, 5, 6, 7, 8};

        ASSERT_EQ(mint(3), a.at(2));

        a.set(3, 10);
        auto expect = std::array<mint, 8>({1, 2, 3, 10, 5, 6, 7, 8});

        ASSERT_EQ(expect, a.to_array());
    }
    {
        // clear
        mintx8 a = {1, 2, 3, 4, 5, 6, 7, 8};
        a.clear(0b01001000);
        auto expect = std::array<mint, 8>({1, 2, 3, 0, 5, 6, 0, 8});
        ASSERT_EQ(expect, a.to_array());
    }
    {
        // minus
        mintx8 a = {1, 2, 3, 4, 5, 6, 7, 8};
        a = -a;
        auto expect = std::array<mint, 8>({-1, -2, -3, -4, -5, -6, -7, -8});
        ASSERT_EQ(expect, a.to_array());
    }
}
