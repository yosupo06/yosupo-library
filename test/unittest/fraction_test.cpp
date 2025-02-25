#include "../utils/int128.hpp"  // IWYU pragma: keep

#include "yosupo/fraction.hpp"
#include "yosupo/int_n.hpp"
#include "yosupo/math.hpp"

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

using FracI = Fraction<int>;
using Frac128 = Fraction<__int128>;
using FracN = Fraction<IntN<10>>;

TEST(FracTest, Constructor) {
    EXPECT_EQ(FracI(), FracI(0, 1));
    EXPECT_EQ(FracI(3), FracI(3, 1));
    EXPECT_EQ(FracI(12, 20), FracI(3, 5));

    EXPECT_EQ(Frac128(12, 20), Frac128(3, 5));
    EXPECT_EQ(FracN(12, 20), FracN(3, 5));
}

TEST(FracTest, Add) {
    {
        FracI f(3, 5);
        FracI g(2, 3);
        EXPECT_EQ(f + g, FracI(19, 15));
    }
    {
        Frac128 f(3, 5);
        Frac128 g(2, 3);
        EXPECT_EQ(f + g, Frac128(19, 15));
    }
    {
        FracN f(3, 5);
        FracN g(2, 3);
        EXPECT_EQ(f + g, FracN(19, 15));
    }
}

TEST(FracTest, Comp) {
    EXPECT_LT(FracI(-1, 3), FracI(-1, 5));
    EXPECT_LT(FracI(1, 5), FracI(1, 3));
    EXPECT_LT(Frac128(-1, 3), Frac128(-1, 5));
    EXPECT_LT(Frac128(1, 5), Frac128(1, 3));
    EXPECT_LT(FracN(-1, 3), FracN(-1, 5));
    EXPECT_LT(FracN(1, 5), FracN(1, 3));
}

TEST(FracTest, ToString) {
    {
        FracI f(3, 5);
        std::ostringstream oss;
        oss << f;
        EXPECT_EQ(oss.str(), "3/5");
    }
    {
        Frac128 f(3, 5);
        std::ostringstream oss;
        oss << f;
        EXPECT_EQ(oss.str(), "3/5");
    }
    {
        FracN f(3, 5);
        std::ostringstream oss;
        oss << f;
        EXPECT_EQ(oss.str(), "3/5");
    }
}

TEST(FracTest, Sign) {
    EXPECT_EQ(FracI(-3, 5).sgn(), -1);
    EXPECT_EQ(FracI(0, 5).sgn(), 0);
    EXPECT_EQ(FracI(3, 5).sgn(), 1);

    EXPECT_EQ(Frac128(-3, 5).sgn(), -1);
    EXPECT_EQ(Frac128(0, 5).sgn(), 0);
    EXPECT_EQ(Frac128(3, 5).sgn(), 1);

    EXPECT_EQ(FracN(-3, 5).sgn(), -1);
    EXPECT_EQ(FracN(0, 5).sgn(), 0);
    EXPECT_EQ(FracN(3, 5).sgn(), 1);
}

TEST(FracTest, Abs) {
    EXPECT_EQ(abs(FracI(3, 5)), FracI(3, 5));
    EXPECT_EQ(abs(FracI(-3, 5)), FracI(3, 5));

    EXPECT_EQ(abs(Frac128(3, 5)), Frac128(3, 5));
    EXPECT_EQ(abs(Frac128(-3, 5)), Frac128(3, 5));

    EXPECT_EQ(abs(FracN(3, 5)), FracN(3, 5));
    EXPECT_EQ(abs(FracN(-3, 5)), FracN(3, 5));
}
