#pragma once

#include <algorithm>
#include <vector>

#include "yosupo/container/vector2d.hpp"

namespace yosupo {

template <class T> int rank(Vector2D<T> a) {
    int h = a.h, w = a.w;
    int r = 0;
    std::vector<int> idxs;
    for (int x = 0; x < w; x++) {
        int my = -1;
        for (int y = r; y < h; y++) {
            if (a[{y, x}] != T(0)) {
                my = y;
                break;
            }
        }
        if (my == -1) continue;
        if (r != my) {
            for (int k = x; k < w; k++) {
                std::swap(a[{r, k}], a[{my, k}]);
            }
        }
        for (int y = r + 1; y < h; y++) {
            if (a[{y, x}] == T(0)) continue;
            auto freq = a[{y, x}] / a[{r, x}];
            for (int k = x; k < w; k++) a[{y, k}] -= freq * a[{r, k}];
        }
        r++;
        idxs.push_back(x);
        if (r == h) break;
    }
    return r;
}

}  // namespace yosupo