#include "yosupo/vector2d.hpp"

#include <gtest/gtest.h>

using namespace yosupo;

TEST(Vector2DTest, Multiply) {
    Vector2D<int> a(2, 3), b(3, 2);
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
    Vector2D<int> c = a * b;
    ASSERT_EQ(c.h, 2);
    ASSERT_EQ(c.w, 2);
    ASSERT_EQ((c[{0, 0}]), 58);
    ASSERT_EQ((c[{0, 1}]), 64);
    ASSERT_EQ((c[{1, 0}]), 139);
    ASSERT_EQ((c[{1, 1}]), 154);
}

TEST(Vector2DTest, Vector2DBool) {
    Vector2D<bool> a(2, 3);
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

TEST(Vector2DTest, Dump) {
    Vector2D<int> a(2, 3);
    a[{0, 0}] = 1;
    a[{0, 1}] = 2;
    a[{0, 2}] = 3;
    a[{1, 0}] = 4;
    a[{1, 1}] = 5;
    a[{1, 2}] = 6;
    ASSERT_EQ(a.dump(), "1 2 3\n4 5 6");
}
