#pragma once

#include <vector>

#include "yosupo/geo/point.hpp"
#include "yosupo/numeric.hpp"

namespace yosupo {

template <class T> using Polygon = std::vector<Point<T>>;

/*
    -1: outside
    0: on line
    1: inside
*/
template <class T> int contains(const Polygon<T>& pol, const Point<T>& p) {
    if (!pol.size()) return 0;
    int in = -1;
    Point<T> _a, _b = pol.back();
    for (auto now : pol) {
        _a = _b;
        _b = now;
        Point<T> a = _a, b = _b;
        if (ccw(a, b, p) == 0) return 0;
        if (a.y > b.y) std::swap(a, b);
        if (!(a.y <= p.y && p.y < b.y)) continue;
        if ((a.y != p.y) ? (sgn(crs(a - p, b - p)) > 0) : (a.x > p.x)) {
            in *= -1;
        }
    }
    return in;
}

}  // namespace yosupo
