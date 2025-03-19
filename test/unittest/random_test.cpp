#include "yosupo/random.hpp"

#include <limits>
#include <random>
#include <vector>

#include "gtest/gtest.h"

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

TEST(RandomTest, OpenClosed01) {
    Random gen(123);
    for (int i = 0; i < 10; i++) {
        double x = open_closed_01();
        ASSERT_GT(x, 0.0);
        ASSERT_LE(x, 1.0);
    }
}
