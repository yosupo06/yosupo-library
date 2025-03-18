#include "yosupo/coord.hpp"
#include "yosupo/util.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

using namespace yosupo;

TEST(CoordTest, DefaultConstructor) {
    Coord c;
    EXPECT_EQ(c, Coord(0, 0));
}

TEST(CoordTest, Eq) {
    Coord c1(1, 2);
    Coord c2(1, 2);
    Coord c3(3, 4);
    EXPECT_EQ(c1, c2);
    EXPECT_NE(c1, c3);
    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);
    EXPECT_FALSE(c1 != c2);
    EXPECT_TRUE(c1 != c3);
}

TEST(CoordTest, ConstructorWithPair) {
    std::pair<int, int> p(3, 4);
    Coord c(p);
    EXPECT_EQ(c, Coord(3, 4));
}

TEST(CoordTest, PlusEqualsOperator) {
    Coord c1(1, 2);
    Coord c2(3, 4);
    c1 += c2;
    EXPECT_EQ(c1, Coord(4, 6));
}

TEST(CoordTest, PlusOperator) {
    Coord c1(1, 2);
    Coord c2(3, 4);
    Coord c3 = c1 + c2;
    EXPECT_EQ(c3, Coord(4, 6));
}

TEST(CoordTest, MinusEqualsOperator) {
    Coord c1(5, 7);
    Coord c2(1, 2);
    c1 -= c2;
    EXPECT_EQ(c1, Coord(4, 5));
}

TEST(CoordTest, MinusOperator) {
    Coord c1(5, 7);
    Coord c2(1, 2);
    Coord c3 = c1 - c2;
    EXPECT_EQ(c3, Coord(4, 5));
}

TEST(CoordTest, IndexOperator) {
    Coord c(1, 2);
    EXPECT_EQ(c[0], 1);
    EXPECT_EQ(c[1], 2);

    c[0] = 3;
    c[1] = 4;
    EXPECT_EQ(c.r(), 3);
    EXPECT_EQ(c.c(), 4);

    const Coord c_const(5, 6);
    EXPECT_EQ(c_const[0], 5);
    EXPECT_EQ(c_const[1], 6);
}

TEST(CoordTest, PairConversion) {
    Coord c(1, 2);
    std::pair<int, int> p = c;
    EXPECT_EQ(p.first, 1);
    EXPECT_EQ(p.second, 2);
}

TEST(CoordTest, Dump) {
    Coord c(1, 2);
    EXPECT_EQ(c.dump(), "(1, 2)");
}

TEST(CoordTest, Move4) {
    Coord c(2, 3);

    EXPECT_EQ(c.move4(0), Coord(2, 4));
    EXPECT_EQ(c.move4(1), Coord(3, 3));
    EXPECT_EQ(c.move4(2), Coord(2, 2));
    EXPECT_EQ(c.move4(3), Coord(1, 3));
}

TEST(CoordTest, Move4Iter) {
    Coord c(5, 5);

    EXPECT_EQ(to_vec(c.move4()), std::vector({Coord(5, 6), Coord(6, 5),
                                              Coord(5, 4), Coord(4, 5)}));
}

TEST(CoordTest, Move8) {
    Coord c(2, 3);

    EXPECT_EQ(c.move8(0), Coord(2, 4));
    EXPECT_EQ(c.move8(1), Coord(3, 4));
    EXPECT_EQ(c.move8(2), Coord(3, 3));
    EXPECT_EQ(c.move8(3), Coord(3, 2));
    EXPECT_EQ(c.move8(4), Coord(2, 2));
    EXPECT_EQ(c.move8(5), Coord(1, 2));
    EXPECT_EQ(c.move8(6), Coord(1, 3));
    EXPECT_EQ(c.move8(7), Coord(1, 4));
}

TEST(CoordTest, Move8Iter) {
    Coord c(5, 5);

    EXPECT_EQ(
        to_vec(c.move8()),
        std::vector({Coord(5, 6), Coord(6, 6), Coord(6, 5), Coord(6, 4),
                     Coord(5, 4), Coord(4, 4), Coord(4, 5), Coord(4, 6)}));
}

TEST(CoordTest, Contain) {
    Coord size(5, 7);

    EXPECT_TRUE(size.contain(Coord(0, 0)));
    EXPECT_TRUE(size.contain(Coord(4, 6)));
    EXPECT_TRUE(size.contain(Coord(2, 3)));

    EXPECT_FALSE(size.contain(Coord(-1, 0)));
    EXPECT_FALSE(size.contain(Coord(0, -1)));
    EXPECT_FALSE(size.contain(Coord(5, 0)));
    EXPECT_FALSE(size.contain(Coord(0, 7)));
    EXPECT_FALSE(size.contain(Coord(5, 7)));
}

TEST(CoordTest, Cells) {
    Coord size(2, 3);

    std::vector<Coord> expected = {Coord(0, 0), Coord(0, 1), Coord(0, 2),
                                   Coord(1, 0), Coord(1, 1), Coord(1, 2)};

    EXPECT_EQ(to_vec(size.cells()), expected);
}
