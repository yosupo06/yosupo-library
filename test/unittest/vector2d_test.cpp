#include "yosupo/vector2d.hpp"

#include <gtest/gtest.h>

using namespace yosupo;

TEST(Vector2DTest, Multiply) {
    {
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
        ASSERT_EQ(c.d[0], 58);
        ASSERT_EQ(c.d[1], 64);
        ASSERT_EQ(c.d[2], 139);
        ASSERT_EQ(c.d[3], 154);
    }
    {
        Vector2D<int> a(2, 3), b(3, 2);
        a[{0, 0}] = 1;
        a[{0, 1}] = 2;
        a[{0, 2}] = 3;
        a[{1, 0}] = 4;
        a[{1, 1}] = 5;
        a[{1, 2}] = 6;
        Vector2D<int> c = a * Vector2D<int>::e(3);
        ASSERT_EQ(c.h, a.h);
        ASSERT_EQ(c.w, a.w);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 3; j++) {
                ASSERT_EQ(c.d[i * c.w + j], a.d[i * a.w + j]);
            }
        }
    }
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
