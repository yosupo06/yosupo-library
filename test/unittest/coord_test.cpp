#include "yosupo/coord.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

using namespace yosupo;

TEST(CoordTest, DefaultConstructor) {
    Coord c;
    ASSERT_EQ(c.r(), 0);
    ASSERT_EQ(c.c(), 0);
}

TEST(CoordTest, ConstructorWithValues) {
    Coord c(1, 2);
    ASSERT_EQ(c.r(), 1);
    ASSERT_EQ(c.c(), 2);
}

TEST(CoordTest, ConstructorWithPair) {
    std::pair<int, int> p(3, 4);
    Coord c(p);
    ASSERT_EQ(c.r(), 3);
    ASSERT_EQ(c.c(), 4);
}

TEST(CoordTest, PlusEqualsOperator) {
    Coord c1(1, 2);
    Coord c2(3, 4);
    c1 += c2;
    ASSERT_EQ(c1.r(), 4);
    ASSERT_EQ(c1.c(), 6);
}

TEST(CoordTest, PlusOperator) {
    Coord c1(1, 2);
    Coord c2(3, 4);
    Coord c3 = c1 + c2;
    ASSERT_EQ(c3.r(), 4);
    ASSERT_EQ(c3.c(), 6);
}

TEST(CoordTest, MinusEqualsOperator) {
    Coord c1(5, 7);
    Coord c2(1, 2);
    c1 -= c2;
    ASSERT_EQ(c1.r(), 4);
    ASSERT_EQ(c1.c(), 5);
}

TEST(CoordTest, MinusOperator) {
    Coord c1(5, 7);
    Coord c2(1, 2);
    Coord c3 = c1 - c2;
    ASSERT_EQ(c3.r(), 4);
    ASSERT_EQ(c3.c(), 5);
}

TEST(CoordTest, IndexOperator) {
    Coord c(1, 2);
    ASSERT_EQ(c[0], 1);
    ASSERT_EQ(c[1], 2);

    c[0] = 3;
    c[1] = 4;
    ASSERT_EQ(c.r(), 3);
    ASSERT_EQ(c.c(), 4);

    const Coord c_const(5, 6);
    ASSERT_EQ(c_const[0], 5);
    ASSERT_EQ(c_const[1], 6);
}

TEST(CoordTest, PairConversion) {
    Coord c(1, 2);
    std::pair<int, int> p = c;
    ASSERT_EQ(p.first, 1);
    ASSERT_EQ(p.second, 2);
}

TEST(CoordTest, Dump) {
    Coord c(1, 2);
    ASSERT_EQ(c.dump(), "(1, 2)");
}

TEST(CoordTest, Move) {
    Coord c(2, 3);

    // dir=0: 右に移動
    Coord right = c.move(0);
    EXPECT_EQ(right.r(), 2);
    EXPECT_EQ(right.c(), 4);

    // dir=1: 下に移動
    Coord down = c.move(1);
    EXPECT_EQ(down.r(), 3);
    EXPECT_EQ(down.c(), 3);

    // dir=2: 左に移動
    Coord left = c.move(2);
    EXPECT_EQ(left.r(), 2);
    EXPECT_EQ(left.c(), 2);

    // dir=3: 上に移動
    Coord up = c.move(3);
    EXPECT_EQ(up.r(), 1);
    EXPECT_EQ(up.c(), 3);
}

TEST(CoordTest, Move4) {
    Coord c(5, 5);

    // move4()を使用して4方向の移動をイテレート
    std::vector<Coord> neighbors;
    for (auto np : c.move4()) {
        neighbors.push_back(np);
    }

    // 4つの隣接点があることを確認
    EXPECT_EQ(neighbors.size(), 4);

    // 各方向の移動が正しいことを確認
    EXPECT_EQ(neighbors[0].r(), 5);
    EXPECT_EQ(neighbors[0].c(), 6);  // 右

    EXPECT_EQ(neighbors[1].r(), 6);
    EXPECT_EQ(neighbors[1].c(), 5);  // 下

    EXPECT_EQ(neighbors[2].r(), 5);
    EXPECT_EQ(neighbors[2].c(), 4);  // 左

    EXPECT_EQ(neighbors[3].r(), 4);
    EXPECT_EQ(neighbors[3].c(), 5);  // 上
}
