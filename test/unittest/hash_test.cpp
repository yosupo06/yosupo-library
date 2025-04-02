#include "yosupo/hash.hpp"

#include <array>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/types.hpp"

using namespace yosupo;

TEST(HashTest, Usage) {
    Hasher h;

    using V = std::vector<int>;
    EXPECT_EQ(h(V{1, 2, 3}), h(V{1, 2, 3}));

    using A = std::array<int, 2>;
    h(A{1, 2});

    using M = std::map<int, int>;
    M m;
    m[1] = 2;
    m[3] = 2;
    h(m);
}

TEST(HashTest, Bool) {
    Hasher h;

    EXPECT_EQ(h(false), h(false));
    EXPECT_EQ(h(true), h(true));
}

TEST(HashTest, Char) {
    Hasher h;

    EXPECT_EQ(h('a'), h('a'));
}

TEST(HashTest, Integers) {
    Hasher h;

    EXPECT_EQ(h(0), h(0));
    EXPECT_EQ(h(1), h(1));
    EXPECT_EQ(h(-1), h(-1));
    EXPECT_EQ(h(123), h(123));
    EXPECT_EQ(h(-123), h(-123));
    EXPECT_EQ(h(std::numeric_limits<int>::min()),
              h(std::numeric_limits<int>::min()));
    EXPECT_EQ(h(std::numeric_limits<int>::max()),
              h(std::numeric_limits<int>::max()));

    EXPECT_EQ(h(static_cast<i8>(42)), h(static_cast<i8>(42)));
    EXPECT_EQ(h(static_cast<u8>(42)), h(static_cast<u8>(42)));
    EXPECT_EQ(h(static_cast<i16>(42)), h(static_cast<i16>(42)));
    EXPECT_EQ(h(static_cast<u16>(42)), h(static_cast<u16>(42)));
    EXPECT_EQ(h(static_cast<i32>(42)), h(static_cast<i32>(42)));
    EXPECT_EQ(h(static_cast<u32>(42)), h(static_cast<u32>(42)));
    EXPECT_EQ(h(static_cast<i64>(42)), h(static_cast<i64>(42)));
    EXPECT_EQ(h(static_cast<u64>(42)), h(static_cast<u64>(42)));
    EXPECT_EQ(h(static_cast<i128>(42)), h(static_cast<i128>(42)));
    EXPECT_EQ(h(static_cast<u128>(42)), h(static_cast<u128>(42)));
}

TEST(HashTest, Pair) {
    Hasher h;

    using P = std::pair<int, u64>;
    EXPECT_EQ(h(P(123, 1)), h(P(123, 1)));
}

TEST(HashTest, Tuple) {
    Hasher h;
    {
        using T = std::tuple<int, bool, u64>;
        EXPECT_EQ(h(T(123, false, 1)), h(T(123, false, 1)));
    }
    {
        EXPECT_NE(h(std::make_tuple(0, 0, 1)), h(std::make_tuple(0, 1, 0)));
        EXPECT_NE(h(std::make_tuple(0, 0, 1)), h(std::make_tuple(1, 0, 0)));
        EXPECT_NE(h(std::make_tuple(0, 1, 0)), h(std::make_tuple(1, 0, 0)));
    }
}

TEST(HashTest, Vector) {
    Hasher h;

    using V = std::vector<int>;
    EXPECT_EQ(h(V{1, 2, 3}), h(V{1, 2, 3}));
}

TEST(HashTest, ShortArray) {
    Hasher h;

    using A = std::array<int, 2>;
    EXPECT_EQ(h(A{1, 2}), h(A{1, 2}));
}
TEST(HashTest, LongArray) {
    Hasher h;

    {
        using A = std::array<int, 50>;
        A a;
        a.fill(1);
        auto x = h(make_tuple(a, a, a));
        auto y = h(make_tuple(a, a, a));
        EXPECT_EQ(x, y);
    }
    {
        using A = std::array<int, 10000>;
        A a;
        a.fill(1);
        auto x = h(a);
        auto y = h(a);
        EXPECT_EQ(x, y);
    }
}

TEST(HashTest, ArrayHashQuality) {
    Hasher h;

    using A = std::array<int, 1000>;
    std::set<u64> hashes;
    for (int i = 0; i < 1000; i++) {
        A t;
        t.fill(0);
        t[i] = 1;
        hashes.insert(h(t));
    }
    EXPECT_GT(hashes.size(), 900);
}

TEST(HashTest, Complex) {
    Hasher h;

    {
        using T = std::pair<std::tuple<int, int>, std::tuple<int, int>>;
        h(T({1, 2}, {3, 4}));
    }
    {
        using T = std::tuple<std::pair<int, int>, std::pair<int, int>>;
        h(T({1, 2}, {3, 4}));
    }
    {
        using T = std::pair<std::vector<int>, std::vector<int>>;
        h(T({1, 2}, {3, 4}));
    }
}

TEST(HashTest, OtherHasher) {
    Hasher h1, h2;

    // should be passed with high probability
    EXPECT_NE(h1(123), h2(123));
}

TEST(HashTest, UnorderedMap) {
    std::unordered_map<std::string, int, Hasher> m;

    m[std::string("abc")] = 123;
    EXPECT_TRUE(m.contains("abc"));
    EXPECT_FALSE(m.contains("cba"));
}
