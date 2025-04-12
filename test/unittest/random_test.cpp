#include "yosupo/random.hpp"

#include <limits>
#include <random>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/modint.hpp"
#include "yosupo/types.hpp"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(RandomTest, Usage) {
    {
        Random gen(123);
        for (int n = 1; n <= 30; n++) {
            std::vector<bool> pop(n);
            for (int ph = 0; ph < 3000; ph++) {
                int x = std::uniform_int_distribution<int>(0, n - 1)(gen);
                ASSERT_LE(0, x);
                ASSERT_GE(99, x);
                pop[x] = true;
            }
            for (int i = 0; i < n; i++) {
                ASSERT_TRUE(pop[i]);
            }
        }
    }
    {
        Random gen(234);
        for (int n = 1; n <= 30; n++) {
            std::vector<bool> pop(n);
            for (int ph = 0; ph < 3000; ph++) {
                int x = uniform(0, n - 1, gen);
                ASSERT_LE(0, x);
                ASSERT_GE(99, x);
                pop[x] = true;
            }
            for (int i = 0; i < n; i++) {
                ASSERT_TRUE(pop[i]);
            }
        }
    }
    {
        for (int n = 1; n <= 30; n++) {
            std::vector<bool> pop(n);
            for (int ph = 0; ph < 3000; ph++) {
                int x = uniform(0, n - 1);
                ASSERT_LE(0, x);
                ASSERT_GE(99, x);
                pop[x] = true;
            }
            for (int i = 0; i < n; i++) {
                ASSERT_TRUE(pop[i]);
            }
        }
    }
}

TEST(RandomTest, UniformBool) {
    for (int phase = 0; phase < 10; phase++) {
        bool f0 = false, f1 = false;
        for (int i = 0; i < 100; i++) {
            if (uniform_bool())
                f1 = true;
            else
                f0 = true;
        }
        ASSERT_TRUE(f0);
        ASSERT_TRUE(f1);
    }
    {
        ull result[2] = {};
        for (int phase = 0; phase < 2; phase++) {
            Random gen(123);
            ull x = 0;
            for (int i = 0; i < 50; i++) {
                x *= 2;
                if (uniform_bool(gen)) x++;
            }
            result[phase] = x;
        }
        for (int phase = 0; phase < 2; phase++) {
            ASSERT_EQ(result[0], result[phase]);
        }
    }
}

TEST(RandomTest, UniformPair) {
    for (int n = 2; n <= 10; n++) {
        std::vector<std::vector<bool>> pop(n, std::vector<bool>(n));
        for (int ph = 0; ph < 10000; ph++) {
            auto p = uniform_pair(0, n - 1);
            ASSERT_LE(0, p.first);
            ASSERT_LE(p.first, p.second);
            ASSERT_LE(p.second, n - 1);
            pop[p.first][p.second] = true;
        }
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                ASSERT_TRUE(pop[i][j]);
            }
        }
    }
}

TEST(RandomTest, UniformMinMax) {
    uniform(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}

TEST(RandomTest, Random01) {
    Random gen(123);
    for (int i = 0; i < 10; i++) {
        double x = random_01();
        ASSERT_LE(0.0, x);
        ASSERT_LT(x, 1.0);
    }
}

TEST(RandomTest, Random) {
    Random gen(123);
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            double x = random(0, i);
            ASSERT_LE(0, x);
            ASSERT_LE(x, i);
        }
    }
}

TEST(RandomTest, UniformCompatibility) {
    WYRand gen(123);
    EXPECT_EQ(internal::uniform_u64(123, gen), 90);
    EXPECT_EQ(internal::uniform_u64(0, gen), 0);
    EXPECT_EQ(internal::uniform_u64(1, gen), 1);
    EXPECT_EQ(internal::uniform_u64(1023, gen), 448);
    EXPECT_EQ(internal::uniform_u64(1024, gen), 629);
    EXPECT_EQ(internal::uniform_u64(u64(1) << 63, gen), 9132492245186335750u);
    EXPECT_EQ(internal::uniform_u64(u64(-1), gen), 15072721646345668731u);
    EXPECT_EQ(internal::uniform_u64(12345, gen), 5576);
}

TEST(RandomTest, UniformAll) {
    {
        using T = i32;
        Random gen1(234), gen2(234);
        u64 x = uniform<T>(gen1);
        u64 y = uniform(std::numeric_limits<T>::min(),
                        std::numeric_limits<T>::max(), gen2);
        EXPECT_EQ(x, y);
    }
    {
        using T = u32;
        Random gen1(234), gen2(234);
        u64 x = uniform<T>(gen1);
        u64 y = uniform(std::numeric_limits<T>::min(),
                        std::numeric_limits<T>::max(), gen2);
        EXPECT_EQ(x, y);
    }
    {
        using T = i64;
        Random gen1(234), gen2(234);
        u64 x = uniform<T>(gen1);
        u64 y = uniform(std::numeric_limits<T>::min(),
                        std::numeric_limits<T>::max(), gen2);
        EXPECT_EQ(x, y);
    }
    {
        using T = u64;
        Random gen1(234), gen2(234);
        u64 x = uniform<T>(gen1);
        u64 y = uniform(std::numeric_limits<T>::min(),
                        std::numeric_limits<T>::max(), gen2);
        EXPECT_EQ(x, y);
    }
}

TEST(RandomTest, UniformMint) {
    using mint = ModInt998244353;
    Random gen(123);
    for (int i = 0; i < 100; i++) {
        mint x = uniform<mint>(gen);
        ASSERT_LE(0, x);
        ASSERT_LE(x, mint::mod() - 1);
    }
}
