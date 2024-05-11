#pragma once

#include <cassert>
#include <vector>

namespace yosupo {

namespace internal {

template <class T>
struct CombState {
    int size = 1;
    std::vector<T> fact = {T(1)};
    std::vector<T> inv_fact = {T(1)};
    std::vector<T> inv = {T(0)};

    void extend() {
        fact.resize(2 * size);
        inv_fact.resize(2 * size);
        inv.resize(2 * size);
        for (int i = size; i < 2 * size; i++) {
            fact[i] = fact[i - 1] * T(i);
        }
        inv_fact[2 * size - 1] = fact[2 * size - 1].inv();
        for (int i = 2 * size - 1; i >= size + 1; i--) {
            inv_fact[i - 1] = inv_fact[i] * T(i);
        }

        for (int i = size; i < 2 * size; i++) {
            inv[i] = inv_fact[i] * fact[i - 1];
        }

        size *= 2;
    }
};

template <class T> CombState<T>& get_comb_state(int n) {
    static CombState<T> state;
    while (state.size <= n) state.extend();
    return state;
}

}

template <class T> T fact(int x) {
    assert(0 <= x);
    return internal::get_comb_state<T>(x).fact[x];
}

template <class T> T inv_fact(int x) {
    assert(0 <= x);
    return internal::get_comb_state<T>(x).inv_fact[x];
}

template <class T> T inv(int x) {
    assert(0 <= x);
    return internal::get_comb_state<T>(x).inv[x];
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
