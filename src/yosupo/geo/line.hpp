#pragma once

#include <algorithm>
#include <deque>
#include <iostream>

#include "yosupo/geo/point.hpp"

namespace yosupo {

template <class T> struct Line {
    Point<T> s, t;
    Point<T> vec() const { return t - s; }

    friend std::ostream& operator<<(std::ostream& os, const Line& l) {
        return os << "L(" << l.s << ", " << l.t << ")";
    }
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

// 半平面の共通部分を求める、半平面はLine lによってccw(l.s, l.t, x) !=
// -1となる点の集合
template <class T>
std::vector<Line<T>> halfplane_intersection(std::vector<Line<T>> lines) {
    std::ranges::sort(lines, [&](const auto& a, const auto& b) {
        auto c = cmp_arg(a.vec(), b.vec());
        if (c != 0) return c < 0;
        // 角度が同じ場合は半平面が大きい方を前に
        return ccw(a.s, a.t, b.s) == 1;
    });
    lines.erase(unique(lines.begin(), lines.end(),
                       [&](const auto& a, const auto& b) {
                           return cmp_arg(a.vec(), b.vec()) == 0;
                       }),
                lines.end());

    std::deque<Line<T>> st;
    for (const auto& l : lines) {
        bool err = false;
        auto is_need = [&](const Line<T>& a, const Line<T>& b,
                           const Line<T>& c) {
            T ab_dw = crs(a.vec(), b.vec()), ab_up = crs(a.vec(), a.t - b.s);
            T bc_dw = crs(b.vec(), c.vec()), bc_up = crs(c.t - b.s, c.vec());
            if (sgn(ab_dw) <= 0 || sgn(bc_dw) <= 0) return true;
            bool f = bc_up * ab_dw > bc_dw * ab_up;
            if (!f && sgn(crs(a.vec(), c.vec())) < 0) err = true;
            return f;
        };
        while (st.size() >= 2 && !is_need(l, st[0], st[1])) st.pop_front();
        while (st.size() >= 2 &&
               !is_need(st[st.size() - 2], st[st.size() - 1], l))
            st.pop_back();
        if (st.size() < 2 || is_need(st.back(), l, st.front())) st.push_back(l);
        if (err) return {};
    }
    if (st.size() == 2 && !sgn(crs(st[0].vec(), st[1].vec())) &&
        sgn(crs(st[0].vec(), st[1].s - st[0].s)) < 0)
        return {};

    return {st.begin(), st.end()};
}

}  // namespace yosupo
