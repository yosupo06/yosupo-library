#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

#include "yosupo/geo/point.hpp"
#include "yosupo/geo/polygon.hpp"
#include "yosupo/util.hpp"

namespace yosupo {

template <class T>
Polygon<T> convex_down(const std::vector<Point<T>>& sorted_ps) {
    assert(sorted_ps.size() >= 2);
    Polygon<T> pol;
    for (auto d : sorted_ps) {
        size_t n;
        while ((n = pol.size()) > 1) {
            if (ccw(pol[n - 2], pol[n - 1], d) == 1) break;
            pol.pop_back();
        }
        pol.push_back(d);
    }
    return pol;
}

template <class T> Polygon<T> convex(std::vector<Point<T>> ps) {
    std::ranges::sort(ps);
    dedup(ps);
    if (ps.size() <= 1) return ps;
    Polygon<T> down = convex_down(ps);
    reverse(ps.begin(), ps.end());
    Polygon<T> up = convex_down(ps);
    down.insert(down.end(), up.begin() + 1, up.end() - 1);
    return down;
}

}  // namespace yosupo
