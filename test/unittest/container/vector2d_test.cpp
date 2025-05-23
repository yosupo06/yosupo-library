#include "yosupo/container/vector2d.hpp"

#include <gtest/gtest.h>

using namespace yosupo;

TEST(Vector2DTest, Empty) {
    Vector2D<int> a;
    ASSERT_EQ(a.h, 0);
    ASSERT_EQ(a.w, 0);
}

TEST(Vector2DTest, Init) {
    Vector2D<int> a(2, 3, 7);
    ASSERT_EQ((a[{0, 0}]), 7);
    ASSERT_EQ((a[{0, 1}]), 7);
    ASSERT_EQ((a[{0, 2}]), 7);
    ASSERT_EQ((a[{1, 0}]), 7);
    ASSERT_EQ((a[{1, 1}]), 7);
    ASSERT_EQ((a[{1, 2}]), 7);
}

TEST(Vector2DTest, Assign) {
    Vector2D<int> a(1, 2);
    a[{0, 0}] = 1;
    a[{0, 1}] = 2;
    auto b = a;
    ASSERT_EQ(b.h, 1);
    ASSERT_EQ(b.w, 2);
    ASSERT_EQ((b[{0, 0}]), 1);
    ASSERT_EQ((b[{0, 1}]), 2);
}

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

TEST(Vector2DTest, InitializerList) {
    Vector2D<int> a = {{1, 2, 3}, {4, 5, 6}};
    ASSERT_EQ(a.h, 2);
    ASSERT_EQ(a.w, 3);
    ASSERT_EQ((a[{0, 0}]), 1);
    ASSERT_EQ((a[{0, 1}]), 2);
    ASSERT_EQ((a[{0, 2}]), 3);
    ASSERT_EQ((a[{1, 0}]), 4);
    ASSERT_EQ((a[{1, 1}]), 5);
    ASSERT_EQ((a[{1, 2}]), 6);

    Vector2D<int> b = {};
    ASSERT_EQ(b.h, 0);
    ASSERT_EQ(b.w, 0);

    Vector2D<int> c = {{1, 2}, {3, 4}};
    ASSERT_EQ(c.h, 2);
    ASSERT_EQ(c.w, 2);
    ASSERT_EQ((c[{0, 0}]), 1);
    ASSERT_EQ((c[{0, 1}]), 2);
    ASSERT_EQ((c[{1, 0}]), 3);
    ASSERT_EQ((c[{1, 1}]), 4);
}

TEST(Vector2DTest, UnevenInitializerList) {
    {
        auto f = []() { Vector2D<int> a = {{1, 2, 3}, {4, 5}}; };
        EXPECT_DEATH(f(), ".*");
    }
    {
        auto f = []() { Vector2D<int> b = {{1}, {2, 3, 4}}; };
        EXPECT_DEATH(f(), ".*");
    }
    {
        auto f = []() { Vector2D<int> c = {{1, 2}, {3, 4, 5}, {6}}; };
        EXPECT_DEATH(f(), ".*");
    }
}
