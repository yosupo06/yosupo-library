#pragma once

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

}  // namespace yosupo
