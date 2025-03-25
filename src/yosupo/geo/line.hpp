#pragma once

#include <algorithm>
#include <cassert>
#include <deque>
#include <utility>
#include <vector>

#include "yosupo/geo/point.hpp"
#include "yosupo/util.hpp"

namespace yosupo {

template <class T> struct Line {
    Point<T> s, t;
    Point<T> vec() const { return t - s; }
};

template <class T>
std::pair<int, Point<T>> cross_ll(const Line<T>& l, const Line<T>& r) {
    T cr1 = crs(l.vec(), r.vec()), cr2 = crs(l.vec(), l.t - r.s);
    if (sgn(crs(l.vec(), r.vec())) == 0) {
        if (sgn(crs(l.vec(), l.t - r.s))) return {0, {}};
        return {-1, {}};
    }
    return {1, r.s + r.vec() * cr2 / cr1};
}

namespace internal {

// bが必要かどうか、つまりhalfplane_intersection({a, b, c}) !=
// halfplane_intersection({a, c})を返す
template <class T>
bool halfplane_intersection3(const Line<T>& a,
                             const Line<T>& b,
                             const Line<T>& c) {
    if (sgn(crs(a.vec(), c.vec())) < 0) return true;
    T ab_dw = crs(a.vec(), b.vec()), ab_up = crs(a.vec(), a.t - b.s);
    T bc_dw = crs(b.vec(), c.vec()), bc_up = crs(c.t - b.s, c.vec());
    bool f = bc_up * ab_dw >= bc_dw * ab_up;
    return f;
};

}  // namespace internal

// 半平面の共通部分を求める
// 半平面はLine lによってccw(l.s, l.t, x) != -1となる点の集合
// division free
template <class T>
std::vector<Line<T>> halfplane_intersection(std::vector<Line<T>> lines) {
    std::ranges::sort(lines, [&](const auto& a, const auto& b) {
        auto c = cmp_arg(a.vec(), b.vec());
        if (c != 0) return c < 0;
        // 角度が同じ場合は半平面が大きい方を前に
        return ccw(a.s, a.t, b.s) == 1;
    });
    dedup(lines, [&](const auto& a, const auto& b) {
        return cmp_arg(a.vec(), b.vec()) == 0;
    });

    std::deque<Line<T>> st;
    for (const auto& l : lines) {
        while (st.size() >= 2 &&
               !internal::halfplane_intersection3(l, st[0], st[1]))
            st.pop_front();
        while (st.size() >= 2 && !internal::halfplane_intersection3(
                                     st[st.size() - 2], st[st.size() - 1], l))
            st.pop_back();
        if (st.size() < 2 ||
            internal::halfplane_intersection3(st.back(), l, st.front()))
            st.push_back(l);
    }
    if (st.size() == 2 && !sgn(crs(st[0].vec(), st[1].vec())) &&
        sgn(crs(st[0].vec(), st[1].s - st[0].s)) < 0)
        return {};
    if (st.size() == 3 &&
        !internal::halfplane_intersection3(st[2], {st[1].t, st[1].s}, st[0])) {
        return {};
    }
    return {st.begin(), st.end()};
}

}  // namespace yosupo
