#include "yosupo/container/sushi.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include "gtest/gtest.h"

namespace {
template <class T>
std::vector<T> naive(std::vector<T>& a,
                     const std::vector<yosupo::SushiQuery<T>>& qs) {
    std::vector<T> result;
    for (auto [l, r, x] : qs) {
        for (int i = l; i < r; ++i)
            if (a[i] < x) std::swap(a[i], x);
        result.push_back(x);
    }
    return result;
}
}  // namespace

TEST(SushiTest, UsageAndPointChangesBetweenBatches) {
    yosupo::SushiBatch<int> sushi({3, 1, 4, 2});
    EXPECT_EQ(sushi.apply({{0, 4, 5}}), std::vector<int>({1}));
    EXPECT_EQ(sushi.values(), std::vector<int>({5, 3, 4, 2}));
    sushi.set(0, -3);
    EXPECT_EQ(sushi.get(0), -3);
    EXPECT_EQ(
        sushi.apply(std::vector<yosupo::SushiQuery<int>>{{0, 2, 1}, {2, 2, 8}}),
        std::vector<int>({-3, 8}));
    EXPECT_EQ(sushi.values(), std::vector<int>({1, 3, 4, 2}));
    EXPECT_TRUE(sushi.apply({}).empty());
}

TEST(SushiTest, ExhaustiveSmallArrays) {
    for (int n = 0, count = 1; n <= 5; ++n, count *= 3) {
        std::vector<yosupo::SushiQuery<int>> qs;
        for (int x = -2; x <= 2; ++x)
            for (int l = 0; l <= n; ++l)
                for (int r = l; r <= n; ++r) qs.push_back({l, r, x});
        for (int mask = 0; mask < count; ++mask) {
            std::vector<int> a(n);
            int m = mask;
            for (int& x : a) {
                x = m % 3 - 1;
                m /= 3;
            }
            auto want = a;
            auto expected = naive(want, qs);
            for (int batch : {1, 3, 17, 1000}) {
                yosupo::SushiBatch<int> sushi(a);
                ASSERT_EQ(sushi.apply(qs, batch), expected)
                    << n << ' ' << mask << ' ' << batch;
                ASSERT_EQ(sushi.values(), want);
            }
        }
    }
}

TEST(SushiTest, RandomizedPersistentBatches) {
    std::mt19937_64 rng(12026);
    for (int trial = 0; trial < 1200; ++trial) {
        int n = int(rng() % 140);
        std::vector<long long> a(n);
        for (auto& x : a) x = static_cast<long long>(rng() % 101) - 50;
        yosupo::SushiBatch<long long> sushi(a);
        for (int epoch = 0; epoch < 5; ++epoch) {
            std::vector<yosupo::SushiQuery<long long>> qs;
            for (int j = 0; j < 60; ++j) {
                int l = int(rng() % unsigned(n + 1)),
                    r = int(rng() % unsigned(n + 1));
                if (l > r) std::swap(l, r);
                if (trial % 4 == 0) {
                    l = 0;
                    r = n;
                }
                long long x = static_cast<long long>(rng() % 201) - 100;
                if (trial % 4 == 1) x = epoch * 60 + j + 100;
                if (j == 0) x = std::numeric_limits<long long>::min();
                if (j == 1) x = std::numeric_limits<long long>::max();
                qs.push_back({l, r, x});
            }
            auto expected = naive(a, qs);
            ASSERT_EQ(sushi.apply(qs, trial % 3 == 0 ? 0 : trial % 37 + 1),
                      expected)
                << trial << ' ' << epoch;
            ASSERT_EQ(sushi.values(), a) << trial << ' ' << epoch;
            if (n) {
                int p = int(rng() % unsigned(n));
                long long x = static_cast<long long>(rng() % 1000) - 500;
                a[p] = x;
                sushi.set(p, x);
            }
        }
    }
}

TEST(SushiTest, UnsignedExtremesAndWordBoundaries) {
    std::mt19937_64 rng(409);
    for (int n : {31, 32, 63, 64, 65, 127, 128, 129, 257}) {
        std::vector<std::uint64_t> a(n);
        for (auto& x : a) x = rng();
        std::vector<yosupo::SushiQuery<std::uint64_t>> qs;
        for (int j = 0; j < 500; ++j) {
            int l = int(rng() % unsigned(n + 1)),
                r = int(rng() % unsigned(n + 1));
            if (l > r) std::swap(l, r);
            qs.push_back(
                {l, r,
                 j % 3 ? rng() : std::numeric_limits<std::uint64_t>::max()});
        }
        auto want = a;
        auto expected = naive(want, qs);
        for (int batch : {1, 11, 65, 1000}) {
            yosupo::SushiBatch<std::uint64_t> sushi(a);
            ASSERT_EQ(sushi.apply(qs, batch), expected);
            ASSERT_EQ(sushi.values(), want);
        }
    }
}

TEST(SushiTest, NonemptyZeroSumAndReusedNodes) {
    yosupo::internal::SushiDeltaTrie trie;
    int root = 0;
    for (int j = 0; j < 100; ++j) {
        root = trie.add(root, 0, -1);
        root = trie.add(root, 3, 1);
        ASSERT_NE(root, 0);
        EXPECT_EQ(trie.sum(root), 0);
        auto children = trie.split(root, 1);
        EXPECT_EQ(trie.sum(children[0]), -1);
        EXPECT_EQ(trie.sum(children[1]), 1);
        root = trie.add(root, 0, 1);
        root = trie.add(root, 3, -1);
        EXPECT_EQ(root, 0);
    }
    EXPECT_LE(trie.nodes.size(), 4U);
}

TEST(SushiTest, LongEpochsAndRepeatedRebuilds) {
    std::mt19937_64 rng(2701);
    for (int n : {1000, 4097}) {
        std::vector<int> a(n);
        std::iota(a.begin(), a.end(), 0);
        std::shuffle(a.begin(), a.end(), rng);
        std::vector<yosupo::SushiQuery<int>> qs;
        for (int j = 0; j < 6000; ++j) {
            int l = int(rng() % unsigned(n)), r = int(rng() % unsigned(n));
            if (l > r) std::swap(l, r);
            int x = j % 3 ? n + j : int(rng() % unsigned(n));
            qs.push_back({l, r + 1, x});
        }
        auto want = a;
        auto expected = naive(want, qs);
        for (int batch : {0, 83, 9000}) {
            yosupo::SushiBatch<int> sushi(a);
            ASSERT_EQ(sushi.apply(qs, batch), expected);
            ASSERT_EQ(sushi.values(), want);
        }
    }
}
