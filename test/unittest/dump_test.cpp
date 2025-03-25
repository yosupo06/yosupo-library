#include "yosupo/dump.hpp"
#include "yosupo/modint.hpp"

#include <gtest/gtest.h>
#include <array>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

TEST(DumpTest, String) {
    EXPECT_EQ(yosupo::dump("hello"), "hello");
    EXPECT_EQ(yosupo::dump(std::string("world")), "world");
}

TEST(DumpTest, Integral) {
    EXPECT_EQ(yosupo::dump(10), "10");
    EXPECT_EQ(yosupo::dump(-5), "-5");
    EXPECT_EQ(yosupo::dump(0), "0");
}

TEST(DumpTest, Uint128) {
    EXPECT_EQ(yosupo::dump(static_cast<__uint128_t>(10)), "10");
    EXPECT_EQ(yosupo::dump(static_cast<__uint128_t>(1) << 100),
              "1267650600228229401496703205376");
    EXPECT_EQ(yosupo::dump(static_cast<__uint128_t>(0)), "0");
}

TEST(DumpTest, Int128) {
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(10)), "10");
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(-1) << 100),
              "-1267650600228229401496703205376");
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(0)), "0");
    EXPECT_EQ(yosupo::dump(static_cast<__int128_t>(-10)), "-10");
}

TEST(DumpTest, FloatingPoint) {
    EXPECT_EQ(yosupo::dump(3.14), "3.140000");
    EXPECT_EQ(yosupo::dump(0.0), "0.000000");
}

TEST(DumpTest, Array) {
    std::array<int, 3> a = {1, 2, 3};
    EXPECT_EQ(yosupo::dump(a), "[1, 2, 3]");

    std::array<std::string, 2> b = {"hello", "world"};
    EXPECT_EQ(yosupo::dump(b), "[hello, world]");
}

TEST(DumpTest, Vector) {
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

TEST(DumpTest, Pair) {
    std::pair<int, int> p = {1, 2};
    EXPECT_EQ(yosupo::dump(p), "(1, 2)");

    std::pair<std::string, int> p2 = {"hello", 10};
    EXPECT_EQ(yosupo::dump(p2), "(hello, 10)");
}

TEST(DumpTest, Tuple) {
    std::tuple<int, int, int> t = {1, 2, 3};
    EXPECT_EQ(yosupo::dump(t), "(1, 2, 3)");

    std::tuple<std::string, int, std::vector<int>> t2 = {"hello", 10, {1, 2}};
    EXPECT_EQ(yosupo::dump(t2), "(hello, 10, [1, 2])");

    std::tuple<> t3 = {};
    EXPECT_EQ(yosupo::dump(t3), "()");
}

TEST(DumpTest, ModInt) {
    yosupo::ModInt<1000000007> a = 10;
    EXPECT_EQ(yosupo::dump(a), "10");

    yosupo::ModInt<1000000007> b = -5;
    EXPECT_EQ(yosupo::dump(b), "1000000002");
}

TEST(DumpTest, Map) {
    std::map<int, int> m1 = {{1, 2}, {3, 4}, {5, 6}};
    EXPECT_EQ(yosupo::dump(m1), "{1: 2, 3: 4, 5: 6}");

    std::map<std::string, int> m2 = {
        {"apple", 1}, {"banana", 2}, {"cherry", 3}};
    EXPECT_EQ(yosupo::dump(m2), "{apple: 1, banana: 2, cherry: 3}");

    std::map<int, std::vector<int>> m3 = {{1, {1, 2, 3}}, {2, {4, 5, 6}}};
    EXPECT_EQ(yosupo::dump(m3), "{1: [1, 2, 3], 2: [4, 5, 6]}");

    std::map<int, std::map<int, int>> m4 = {{1, {{1, 2}, {3, 4}}},
                                            {2, {{5, 6}, {7, 8}}}};
    EXPECT_EQ(yosupo::dump(m4), "{1: {1: 2, 3: 4}, 2: {5: 6, 7: 8}}");

    std::map<int, int> m5;
    EXPECT_EQ(yosupo::dump(m5), "{}");
}
