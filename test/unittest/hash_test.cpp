#include "yosupo/hash.hpp"

#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(HashTest, Usage) {
    ASSERT_EQ(UniversalHash32<bool>()(false), UniversalHash32<bool>()(false));
    ASSERT_EQ(UniversalHash32<bool>()(true), UniversalHash32<bool>()(true));
    ASSERT_EQ(UniversalHash32<char>()('a'), UniversalHash32<char>()('a'));
    ASSERT_EQ(UniversalHash32<int>()(123), UniversalHash32<int>()(123));
    ASSERT_EQ(UniversalHash32<int>()(-1), UniversalHash32<int>()(-1));
    ASSERT_EQ(UniversalHash32<unsigned int>()(123),
              UniversalHash32<unsigned int>()(123));
    ASSERT_EQ(UniversalHash32<long>()(123), UniversalHash32<long>()(123));
    ASSERT_EQ(UniversalHash32<long long>()(-1),
              UniversalHash32<long long>()(-1));

    ASSERT_EQ(UniversalHash32<__int128>()(-1), UniversalHash32<__int128>()(-1));

    using P = std::pair<int, unsigned long long>;
    ASSERT_EQ(UniversalHash32<P>()(P(123, 1)), UniversalHash32<P>()(P(123, 1)));

    using T = std::tuple<int, bool, unsigned long long>;
    ASSERT_EQ(UniversalHash32<T>()(T(123, false, 1)),
              UniversalHash32<T>()(T(123, false, 1)));
}
