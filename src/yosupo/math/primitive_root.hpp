#pragma once

#include <cmath>

#include "yosupo/math/basic.hpp"
#include "yosupo/types.hpp"

namespace yosupo {
inline constexpr u32 smallest_primitive_root(u32 m) {
    if (m == 2) return 1;

    u32 divs[20] = {};
    int cnt = 0;
    u32 x = (m - 1) / 2;
    for (int i = 2; (u64)(i)*i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) {
                x /= i;
            }
        }
    }
    if (x > 1) {
        divs[cnt++] = x;
    }
    for (u32 g = 2;; g++) {
        bool ok = true;
        for (int i = 0; i < cnt; i++) {
            if (pow_mod_u64(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}
}  // namespace yosupo