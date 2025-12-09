#include "yosupo/container/vector2d.hpp"

#include <gtest/gtest.h>

#include <utility>

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

TEST(Vector2DTest, Equality) {
    Vector2D<int> a = {{1, 2}, {3, 4}};
    Vector2D<int> b = a;
    ASSERT_TRUE(a == b);

    b[{1, 1}] = 5;
    ASSERT_TRUE(a != b);

    Vector2D<int> c(3, 2);
    ASSERT_FALSE(a == c);
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

TEST(Vector2DTest, Fill) {
    Vector2D<int> a(2, 3);
    a.fill(5);
    ASSERT_EQ((a[{0, 0}]), 5);
    ASSERT_EQ((a[{0, 1}]), 5);
    ASSERT_EQ((a[{0, 2}]), 5);
    ASSERT_EQ((a[{1, 0}]), 5);
    ASSERT_EQ((a[{1, 1}]), 5);
    ASSERT_EQ((a[{1, 2}]), 5);
    a.fill(3);
    ASSERT_EQ((a[{1, 0}]), 3);
    a.fill(111);
    ASSERT_EQ((a[{0, 2}]), 111);
}

TEST(Vector2DTest, MoveAndSwap) {
    Vector2D<int> a(2, 2, 3);
    Vector2D<int> b(1, 1, 9);

    auto a_ptr = a.data();
    auto b_ptr = b.data();

    Vector2D<int> moved = std::move(a);
    EXPECT_EQ(moved.data(), a_ptr);
    EXPECT_EQ(moved.h, 2);
    EXPECT_EQ(moved.w, 2);
    EXPECT_EQ((moved[{1, 1}]), 3);
    EXPECT_EQ(a.data(), nullptr);
    EXPECT_EQ(a.h, 0);
    EXPECT_EQ(a.w, 0);

    std::swap(moved, b);
    EXPECT_EQ(moved.data(), b_ptr);
    EXPECT_EQ(moved.h, 1);
    EXPECT_EQ(moved.w, 1);
    EXPECT_EQ((moved[{0, 0}]), 9);
    EXPECT_EQ(b.data(), a_ptr);
    EXPECT_EQ(b.h, 2);
    EXPECT_EQ(b.w, 2);
    EXPECT_EQ((b[{0, 0}]), 3);
}

TEST(Vector2DTest, Transpose) {
    Vector2D<int> a = {{1, 2, 3}, {4, 5, 6}};
    auto t = a.transpose();
    ASSERT_EQ(t.h, 3);
    ASSERT_EQ(t.w, 2);
    ASSERT_EQ((t[{0, 0}]), 1);
    ASSERT_EQ((t[{0, 1}]), 4);
    ASSERT_EQ((t[{1, 0}]), 2);
    ASSERT_EQ((t[{1, 1}]), 5);
    ASSERT_EQ((t[{2, 0}]), 3);
    ASSERT_EQ((t[{2, 1}]), 6);
}

TEST(Vector2DTest, SwapAndArithmetic) {
    Vector2D<int> a = {{1, 2}, {3, 4}};
    Vector2D<int> b = {{5, 6}, {7, 8}};
    std::swap(a, b);
    ASSERT_EQ((a[{0, 0}]), 5);
    ASSERT_EQ((a[{1, 1}]), 8);
    ASSERT_EQ((b[{0, 0}]), 1);
    ASSERT_EQ((b[{1, 1}]), 4);

    auto c = a + b;
    ASSERT_EQ((c[{0, 0}]), 6);
    ASSERT_EQ((c[{1, 1}]), 12);

    c -= b;
    ASSERT_EQ(c, a);
}

TEST(Vector2DTest, Pow) {
    Vector2D<int> a = {{1, 1}, {1, 0}};
    auto a2 = a.pow(2);
    ASSERT_EQ((a2[{0, 0}]), 2);
    ASSERT_EQ((a2[{0, 1}]), 1);
    ASSERT_EQ((a2[{1, 0}]), 1);
    ASSERT_EQ((a2[{1, 1}]), 1);

    auto a5 = a.pow(5);
    ASSERT_EQ((a5[{0, 0}]), 8);
    ASSERT_EQ((a5[{0, 1}]), 5);
    ASSERT_EQ((a5[{1, 0}]), 5);
    ASSERT_EQ((a5[{1, 1}]), 3);
}
