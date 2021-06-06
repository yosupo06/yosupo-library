#pragma once

#include <vector>

namespace yosupo {

template <class T> T fact(int x) {
    assert(0 <= x);
    static std::vector<T> v = {1};
    while (int(v.size()) <= x) {
        auto z = v.back() * v.size();
        v.push_back(z);
    }
    return v[x];
}

template <class T> T inv_fact(int x) {
    assert(0 <= x);
    static std::vector<T> v = {1};
    while (int(v.size()) <= x) {
        auto z = v.back() * T(v.size()).inv();
        v.push_back(z);
    }
    return v[x];
}

namespace internal {
template <class T> T comb(int n, int k) {
    return fact<T>(n) * inv_fact<T>(k) * inv_fact<T>(n - k);
}
}

template <class T> T comb(int n, int k) {
    assert(0 <= k);
    if (0 <= n && n < k) return 0;    

    if (n >= 0) {
        return internal::comb<T>(n, k);
    }
    T x = internal::comb<T>(k - n - 1, k);
    if (k % 2) x = -x;
    return x;
}

}
