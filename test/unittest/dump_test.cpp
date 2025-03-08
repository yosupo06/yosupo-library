#include "yosupo/dump.hpp"

#include <gtest/gtest.h>

TEST(DebugTest, Integral) {
    EXPECT_EQ(yosupo::dump(10), "10");
    EXPECT_EQ(yosupo::dump(-5), "-5");
    EXPECT_EQ(yosupo::dump(0), "0");
}

TEST(DebugTest, Uint128) {
    EXPECT_EQ(yosupo::dump(static_cast<__uint128_t>(10)), "10");
    EXPECT_EQ(yosupo::dump(static_cast<__uint128_t>(1) << 100),
              "1267650600228229401496703205376");
    EXPECT_EQ(yosupo::dump(static_cast<__uint128_t>(0)), "0");
}

TEST(DebugTest, Int128) {
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(10)), "10");
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(-1) << 100),
              "-1267650600228229401496703205376");
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(0)), "0");
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(-10)), "-10");
}

TEST(DebugTest, Array) {
    std::array<int, 3> a = {1, 2, 3};
    EXPECT_EQ(yosupo::dump(a), "[1, 2, 3]");

    std::array<std::string, 2> b = {"hello", "world"};
    EXPECT_EQ(yosupo::dump(b), "[hello, world]");
}

TEST(DebugTest, Vector) {
    {
        std::vector<int> v = {1, 2, 3, 4, 5};
        EXPECT_EQ(yosupo::dump(v), "[1, 2, 3, 4, 5]");
    }
    {
        std::vector<std::string> v;
        EXPECT_EQ(yosupo::dump(v), "[]");
    }
    {
        std::vector<std::tuple<int, int>> v = {{1, 2}, {3, 4}};
        EXPECT_EQ(yosupo::dump(v), "[(1, 2), (3, 4)]");
    }
}

TEST(DebugTest, Pair) {
    std::pair<int, int> p = {1, 2};
    EXPECT_EQ(yosupo::dump(p), "(1, 2)");

    std::pair<std::string, int> p2 = {"hello", 10};
    EXPECT_EQ(yosupo::dump(p2), "(hello, 10)");
}

TEST(DebugTest, Tuple) {
    std::tuple<int, int, int> t = {1, 2, 3};
    EXPECT_EQ(yosupo::dump(t), "(1, 2, 3)");

    std::tuple<std::string, int, std::vector<int>> t2 = {"hello", 10, {1, 2}};
    EXPECT_EQ(yosupo::dump(t2), "(hello, 10, [1, 2])");

    std::tuple<> t3 = {};
    EXPECT_EQ(yosupo::dump(t3), "()");
}
