#include "yosupo/geo/line.hpp"
#include "yosupo/fraction.hpp"

#include <algorithm>

#include "gtest/gtest.h"

using namespace yosupo;

using R = Fraction<int>;
using P = Point<R>;
using L = Line<R>;

TEST(LineTest, CrossLLIntersecting) {
    L l1{P{0, 0}, P{1, 1}};
    L l2{P{0, 1}, {1, 0}};
    auto res1 = cross_ll(l1, l2);
    ASSERT_EQ(res1.first, 1);
    ASSERT_EQ(res1.second.x, R(1, 2));
    ASSERT_EQ(res1.second.y, R(1, 2));
}
TEST(LineTest, CrossLLParallel) {
    L l3{{0, 0}, {1, 0}};
    L l4{{0, 1}, {1, 1}};
    auto res2 = cross_ll(l3, l4);
    ASSERT_EQ(res2.first, 0);
}
TEST(LineTest, CrossLLCollinear) {
    L l5{{0, 0}, {1, 1}};
    L l6{{2, 2}, {3, 3}};
    auto res3 = cross_ll(l5, l6);
    ASSERT_EQ(res3.first, -1);
}

bool equal_rotate(std::vector<L> src, const std::vector<L>& trg) {
    if (src.size() != src.size()) return false;
    if (trg.empty()) return true;
    const size_t n = src.size();
    for (size_t shift = 0; shift < n; ++shift) {
        bool ok = true;
        for (size_t i = 0; i < n; ++i) {
            if (src[i].s != trg[i].s || src[i].t != trg[i].t) {
                ok = false;
                break;
            }
        }
        if (ok) return true;
        std::rotate(src.begin(), src.begin() + 1, src.end());
    }
    return false;
}

TEST(LineTest, HalfplaneIntersectionEmpty) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{10, -1}, {0, -1}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(res.empty());
}

TEST(LineTest, HalfplaneIntersectionLine) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{10, 0}, {0, 0}}};
    auto res = halfplane_intersection(lines);

    ASSERT_TRUE(equal_rotate(res, lines));
}

TEST(LineTest, HalfplaneIntersectionTriangle) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{10, 0}, {5, 10}},
                            L{{5, 10}, {0, 0}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(equal_rotate(res, lines));
}

TEST(LineTest, HalfplaneIntersectionSameArg) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{3, 1}, {5, 1}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(equal_rotate(res, {L{{0, 0}, {10, 0}}}));
}

TEST(LineTest, HalfplaneIntersectionOpen) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{5, 0}, {10, 5}},
                            L{{10, 0}, {10, 10}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(equal_rotate(res, lines));
}

TEST(LineTest, HalfplaneIntersectionRemove) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{10, 0}, {15, 5}},
                            L{{5, 0}, {5, 10}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(equal_rotate(res, {L{{0, 0}, {10, 0}}, L{{5, 0}, {5, 10}}}));
}

TEST(LineTest, HalfplaneIntersectionRemoveEdge) {
    std::vector<L> lines = {L{{0, 0}, {10, 0}}, L{{10, 0}, {15, 5}},
                            L{{10, 0}, {10, 10}}};
    auto res = halfplane_intersection(lines);
    ASSERT_TRUE(equal_rotate(res, {L{{0, 0}, {10, 0}}, L{{10, 0}, {10, 10}}}));
}
