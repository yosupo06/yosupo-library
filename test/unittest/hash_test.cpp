#include "yosupo/hash.hpp"

#include <array>
#include <limits>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/container/bitvector.hpp"
#include "yosupo/types.hpp"

using namespace yosupo;

TEST(HashTest, Usage) {
    using V = std::vector<int>;
    EXPECT_EQ(Hasher<V>()(V{1, 2, 3}), Hasher<V>()(V{1, 2, 3}));

    using A = std::array<int, 2>;
    Hasher<A>()(A{1, 2});

    using M = std::map<int, int>;
    M m;
    m[1] = 2;
    m[3] = 2;
    Hasher<M>()(m);

    BitVec bs(100);
    Hasher<BitVec>()(bs);
}

TEST(HashTest, Bool) {
    EXPECT_EQ(Hasher<bool>()(false), Hasher<bool>()(false));
    EXPECT_EQ(Hasher<bool>()(true), Hasher<bool>()(true));
}

TEST(HashTest, Char) { EXPECT_EQ(Hasher<char>()('a'), Hasher<char>()('a')); }

TEST(HashTest, Integers) {
    EXPECT_EQ(Hasher<int>()(0), Hasher<int>()(0));
    EXPECT_EQ(Hasher<int>()(1), Hasher<int>()(1));
    EXPECT_EQ(Hasher<int>()(-1), Hasher<int>()(-1));
    EXPECT_EQ(Hasher<int>()(123), Hasher<int>()(123));
    EXPECT_EQ(Hasher<int>()(-123), Hasher<int>()(-123));
    EXPECT_EQ(Hasher<int>()(std::numeric_limits<int>::min()),
              Hasher<int>()(std::numeric_limits<int>::min()));
    EXPECT_EQ(Hasher<int>()(std::numeric_limits<int>::max()),
              Hasher<int>()(std::numeric_limits<int>::max()));

    EXPECT_EQ(Hasher<i8>()(42), Hasher<i8>()(42));
    EXPECT_EQ(Hasher<u8>()(42), Hasher<u8>()(42));
    EXPECT_EQ(Hasher<i16>()(42), Hasher<i16>()(42));
    EXPECT_EQ(Hasher<u16>()(42), Hasher<u16>()(42));
    EXPECT_EQ(Hasher<i32>()(42), Hasher<i32>()(42));
    EXPECT_EQ(Hasher<u32>()(42), Hasher<u32>()(42));
    EXPECT_EQ(Hasher<i64>()(42), Hasher<i64>()(42));
    EXPECT_EQ(Hasher<u64>()(42), Hasher<u64>()(42));
    EXPECT_EQ(Hasher<i128>()(42), Hasher<i128>()(42));
    EXPECT_EQ(Hasher<u128>()(42), Hasher<u128>()(42));
}

TEST(HashTest, Pair) {
    using P = std::pair<int, u64>;
    EXPECT_EQ(Hasher<P>()(P(123, 1)), Hasher<P>()(P(123, 1)));
}

TEST(HashTest, Tuple) {
    using T = std::tuple<int, bool, u64>;
    EXPECT_EQ(Hasher<T>()(T(123, false, 1)), Hasher<T>()(T(123, false, 1)));
}

TEST(HashTest, Vector) {
    using V = std::vector<int>;
    EXPECT_EQ(Hasher<V>()(V{1, 2, 3}), Hasher<V>()(V{1, 2, 3}));
}

TEST(HashTest, ShortArray) {
    using A = std::array<int, 2>;
    EXPECT_EQ(Hasher<A>()(A{1, 2}), Hasher<A>()(A{1, 2}));
}
TEST(HashTest, LongArray) {
    using A = std::array<int, 10000>;
    A t1;
    t1.fill(1);
    A t2 = t1;
    auto x = Hasher<A>()(t1);
    auto y = Hasher<A>()(t2);
    EXPECT_EQ(x, y);
}

TEST(HashTest, ArrayHashQuality) {
    using A = std::array<int, 1000>;
    std::set<u64> hashes;
    for (int i = 0; i < 1000; i++) {
        A t;
        t.fill(0);
        t[i] = 1;
        hashes.insert(Hasher<A>()(t));
    }
    EXPECT_GT(hashes.size(), 900);
}

TEST(HashTest, Complex) {
    {
        using T = std::pair<std::tuple<int, int>, std::tuple<int, int>>;
        Hasher<T>()(T({1, 2}, {3, 4}));
    }
    {
        using T = std::tuple<std::pair<int, int>, std::pair<int, int>>;
        Hasher<T>()(T({1, 2}, {3, 4}));
    }
    {
        using T = std::pair<std::vector<int>, std::vector<int>>;
        Hasher<T>()(T({1, 2}, {3, 4}));
    }
}
