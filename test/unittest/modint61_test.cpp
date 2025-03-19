#include "yosupo/modint61.hpp"

#include "gtest/gtest.h"

TEST(ModInt61Test, Construct) {
    yosupo::ModInt61 a = 2;
    ASSERT_EQ(2, a.val());

    yosupo::ModInt61 b = __int128(1234);
    ASSERT_EQ(1234, b.val());

    yosupo::ModInt61 c = (long long)(12345);
    ASSERT_EQ(12345, c.val());

    yosupo::ModInt61 d = (unsigned __int128)(123456);
    ASSERT_EQ(123456, d.val());
}

TEST(ModInt61Test, Mul) {
    yosupo::ModInt61 a = (1LL << 29);
    yosupo::ModInt61 b = (1LL << 32);
    ASSERT_EQ(1, (a * b).val());
}

TEST(ModInt61Test, Hash) {
    yosupo::RHash a("ab"), b("c"), c("a"), d("bc");

    ASSERT_EQ(a + b, c + d);
    ASSERT_NE(yosupo::RHash("aaa"), yosupo::RHash("aab"));

    ASSERT_EQ((a + b).val(), (c + d).val());
    ASSERT_NE(yosupo::RHash("aaa").val(), yosupo::RHash("aab").val());
}
