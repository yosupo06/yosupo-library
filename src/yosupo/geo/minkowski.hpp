#pragma once

#include <algorithm>
#include <vector>

#include "yosupo/geo/point.hpp"
#include "yosupo/geo/polygon.hpp"

namespace yosupo {

// Minkowski sum of convex polygons, in O(a.size() + b.size()) time and space.
// Inputs are counterclockwise, without repeated vertices or collinear boundary
// vertices (as returned by convex()). Any cyclic starting vertex is allowed.
// Empty polygons, points and segments are supported. The result follows the
// convex() convention: lexicographically smallest vertex first, CCW order,
// without redundant collinear vertices or a repeated closing vertex.
// Coordinate sums, differences and cross products must fit in T.
template <class T>
Polygon<T> minkowski_sum(const Polygon<T>& a, const Polygon<T>& b) {
    if (a.empty() || b.empty()) return {};
    const int n = int(a.size()), m = int(b.size());
    auto first_edge = [](const Polygon<T>& p) {
        const int size = int(p.size());
        int first = 0;
        for (int i = 1; i < size; i++) {
            if (cmp_arg(p[(i + 1) % size] - p[i],
                        p[(first + 1) % size] - p[first]) < 0) {
                first = i;
            }
        }
        return first;
    };
    int i = first_edge(a), j = first_edge(b);
    const int na = n == 1 ? 0 : n, nb = m == 1 ? 0 : m;
    int used_a = 0, used_b = 0;
    Polygon<T> result;
    result.reserve(a.size() + b.size());
    result.push_back(a[i] + b[j]);
    while (used_a < na || used_b < nb) {
        bool advance_a = used_a < na, advance_b = used_b < nb;
        if (advance_a && advance_b) {
            auto cmp = cmp_arg(a[(i + 1) % n] - a[i],
                               b[(j + 1) % m] - b[j]);
            advance_a = cmp <= 0;
            advance_b = cmp >= 0;
        }
        if (advance_a) {
            i = (i + 1) % n;
            used_a++;
        }
        if (advance_b) {
            j = (j + 1) % m;
            used_b++;
        }
        if (used_a < na || used_b < nb) result.push_back(a[i] + b[j]);
    }
    std::rotate(result.begin(),
                std::min_element(result.begin(), result.end()), result.end());
    return result;
}

}  // namespace yosupo
