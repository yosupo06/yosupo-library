#include "yosupo/container/array2d.hpp"

#include <gtest/gtest.h>

using namespace yosupo;

TEST(Array2DTest, Empty) {
    Array2D<int, 0, 0> a;
    ASSERT_EQ(a.height(), 0);
    ASSERT_EQ(a.width(), 0);
}

TEST(Array2DTest, Init) {
    Array2D<int, 2, 3> a(7);
    ASSERT_EQ((a[{0, 0}]), 7);
    ASSERT_EQ((a[{0, 1}]), 7);
    ASSERT_EQ((a[{0, 2}]), 7);
    ASSERT_EQ((a[{1, 0}]), 7);
    ASSERT_EQ((a[{1, 1}]), 7);
    ASSERT_EQ((a[{1, 2}]), 7);
}

TEST(Array2DTest, DefaultInit) {
    Array2D<int, 2, 3> a;
    ASSERT_EQ((a[{0, 0}]), 0);
    ASSERT_EQ((a[{0, 1}]), 0);
    ASSERT_EQ((a[{0, 2}]), 0);
    ASSERT_EQ((a[{1, 0}]), 0);
    ASSERT_EQ((a[{1, 1}]), 0);
    ASSERT_EQ((a[{1, 2}]), 0);
}

TEST(Array2DTest, Assign) {
    Array2D<int, 1, 2> a;
    a[{0, 0}] = 1;
    a[{0, 1}] = 2;
    auto b = a;
    ASSERT_EQ(b.height(), 1);
    ASSERT_EQ(b.width(), 2);
    ASSERT_EQ((b[{0, 0}]), 1);
    ASSERT_EQ((b[{0, 1}]), 2);
}

TEST(Array2DTest, Multiply) {
    Array2D<int, 2, 3> a;
    Array2D<int, 3, 2> b;
    a[{0, 0}] = 1;
    a[{0, 1}] = 2;
    a[{0, 2}] = 3;
    a[{1, 0}] = 4;
    a[{1, 1}] = 5;
    a[{1, 2}] = 6;
    b[{0, 0}] = 7;
    b[{0, 1}] = 8;
    b[{1, 0}] = 9;
    b[{1, 1}] = 10;
    b[{2, 0}] = 11;
    b[{2, 1}] = 12;
    auto c = a * b;
    ASSERT_EQ(c.height(), 2);
    ASSERT_EQ(c.width(), 2);
    ASSERT_EQ((c[{0, 0}]), 58);
    ASSERT_EQ((c[{0, 1}]), 64);
    ASSERT_EQ((c[{1, 0}]), 139);
    ASSERT_EQ((c[{1, 1}]), 154);
}

TEST(Array2DTest, Array2DBool) {
    Array2D<bool, 2, 3> a;
    a[{0, 0}] = true;
    a[{0, 1}] = false;
    a[{0, 2}] = true;
    a[{1, 0}] = false;
    a[{1, 1}] = true;
    a[{1, 2}] = false;
    ASSERT_EQ((a[{0, 0}]), true);
    ASSERT_EQ((a[{0, 1}]), false);
    ASSERT_EQ((a[{0, 2}]), true);
    ASSERT_EQ((a[{1, 0}]), false);
    ASSERT_EQ((a[{1, 1}]), true);
    ASSERT_EQ((a[{1, 2}]), false);
}

TEST(Array2DTest, Dump) {
    Array2D<int, 2, 3> a;
    a[{0, 0}] = 1;
    a[{0, 1}] = 2;
    a[{0, 2}] = 3;
    a[{1, 0}] = 4;
    a[{1, 1}] = 5;
    a[{1, 2}] = 6;
    ASSERT_EQ(a.dump(), "1 2 3\n4 5 6");
}

TEST(Array2DTest, InitializerList) {
    Array2D<int, 2, 3> a = {{1, 2, 3}, {4, 5, 6}};
    ASSERT_EQ(a.height(), 2);
    ASSERT_EQ(a.width(), 3);
    ASSERT_EQ((a[{0, 0}]), 1);
    ASSERT_EQ((a[{0, 1}]), 2);
    ASSERT_EQ((a[{0, 2}]), 3);
    ASSERT_EQ((a[{1, 0}]), 4);
    ASSERT_EQ((a[{1, 1}]), 5);
    ASSERT_EQ((a[{1, 2}]), 6);

    Array2D<int, 0, 0> b = {};
    ASSERT_EQ(b.height(), 0);
    ASSERT_EQ(b.width(), 0);

    Array2D<int, 2, 2> c = {{1, 2}, {3, 4}};
    ASSERT_EQ(c.height(), 2);
    ASSERT_EQ(c.width(), 2);
    ASSERT_EQ((c[{0, 0}]), 1);
    ASSERT_EQ((c[{0, 1}]), 2);
    ASSERT_EQ((c[{1, 0}]), 3);
    ASSERT_EQ((c[{1, 1}]), 4);
}

TEST(Array2DTest, Addition) {
    Array2D<int, 2, 2> a = {{1, 2}, {3, 4}};
    Array2D<int, 2, 2> b = {{5, 6}, {7, 8}};
    auto c = a + b;
    ASSERT_EQ((c[{0, 0}]), 6);
    ASSERT_EQ((c[{0, 1}]), 8);
    ASSERT_EQ((c[{1, 0}]), 10);
    ASSERT_EQ((c[{1, 1}]), 12);
}

TEST(Array2DTest, Subtraction) {
    Array2D<int, 2, 2> a = {{5, 6}, {7, 8}};
    Array2D<int, 2, 2> b = {{1, 2}, {3, 4}};
    auto c = a - b;
    ASSERT_EQ((c[{0, 0}]), 4);
    ASSERT_EQ((c[{0, 1}]), 4);
    ASSERT_EQ((c[{1, 0}]), 4);
    ASSERT_EQ((c[{1, 1}]), 4);
}

TEST(Array2DTest, Identity) {
    auto e = Array2D<int, 3, 3>::e();
    ASSERT_EQ((e[{0, 0}]), 1);
    ASSERT_EQ((e[{0, 1}]), 0);
    ASSERT_EQ((e[{0, 2}]), 0);
    ASSERT_EQ((e[{1, 0}]), 0);
    ASSERT_EQ((e[{1, 1}]), 1);
    ASSERT_EQ((e[{1, 2}]), 0);
    ASSERT_EQ((e[{2, 0}]), 0);
    ASSERT_EQ((e[{2, 1}]), 0);
    ASSERT_EQ((e[{2, 2}]), 1);
}

TEST(Array2DTest, Transpose) {
    Array2D<int, 2, 3> a = {{1, 2, 3}, {4, 5, 6}};
    auto b = a.transpose();
    ASSERT_EQ(b.height(), 3);
    ASSERT_EQ(b.width(), 2);
    ASSERT_EQ((b[{0, 0}]), 1);
    ASSERT_EQ((b[{0, 1}]), 4);
    ASSERT_EQ((b[{1, 0}]), 2);
    ASSERT_EQ((b[{1, 1}]), 5);
    ASSERT_EQ((b[{2, 0}]), 3);
    ASSERT_EQ((b[{2, 1}]), 6);
}

TEST(Array2DTest, Power) {
    Array2D<int, 2, 2> a = {{2, 1}, {1, 1}};
    auto b = a.pow(3);
    ASSERT_EQ((b[{0, 0}]), 13);
    ASSERT_EQ((b[{0, 1}]), 8);
    ASSERT_EQ((b[{1, 0}]), 8);
    ASSERT_EQ((b[{1, 1}]), 5);
}