#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "yosupo/internal_type_traits.hpp"
#include "yosupo/random.hpp"

namespace yosupo {

namespace internal {

constexpr int THRESHOLD_HASH = 100;

// Reference: Lemire, Daniel., and Owen, Kaser.
// Strongly Universal String Hashing Is Fast.
uint64_t get_seed(int i) {
    static std::array<uint64_t, THRESHOLD_HASH> seed = []() {
        std::array<uint64_t, THRESHOLD_HASH> _seed;
        for (auto& x : _seed) {
            x = yosupo::uniform(uint64_t(0), uint64_t(-1));
        }
        return _seed;
    }();
    static std::vector<uint64_t> long_seed;

    if (i < THRESHOLD_HASH) return seed[i];

    while (THRESHOLD_HASH + int(long_seed.size()) <= i) {
        long_seed.push_back(yosupo::uniform(uint64_t(0), uint64_t(-1)));
    }

    return long_seed[i - THRESHOLD_HASH];
}

template <int I = 1> struct Hasher32 {
    uint64_t v = get_seed(0);

    template <class T,
              is_integral_t<T>* = nullptr,
              std::enable_if_t<sizeof(T) == 4>* = nullptr>
    Hasher32<I + 1> update(T x) const {
        return Hasher32<I + 1>{v + uint32_t(x) * get_seed(I)};
    }
    template <class T,
              is_integral_t<T>* = nullptr,
              std::enable_if_t<sizeof(T) == 1>* = nullptr>
    Hasher32<I + 1> update(T x) const {
        return update(uint32_t(x));
    }
    template <class T,
              is_integral_t<T>* = nullptr,
              std::enable_if_t<sizeof(T) == 8>* = nullptr>
    Hasher32<I + 2> update(T x) const {
        return update(uint32_t(x)).update(uint32_t(uint64_t(x) >> 32));
    }
    template <class T,
              is_integral_t<T>* = nullptr,
              std::enable_if_t<sizeof(T) == 16>* = nullptr>
    Hasher32<I + 4> update(T x) const {
        return update(uint64_t(x)).update(uint64_t((__uint128_t)(x) >> 64));
    }
    uint32_t digest() const { return (v >> 32); }
};

template <class T, is_integral_t<T>* = nullptr> uint32_t hash32(const T& x) {
    return Hasher32<>{}.update(x).digest();
}

template <class S, class T> uint32_t hash32(const std::pair<S, T>& x) {
    return Hasher32<>{}.update(x.first).update(x.second).digest();
}

template <int I = 0,
          class H,
          class T,
          std::enable_if_t<(I != std::tuple_size<T>::value)>* = nullptr>
uint32_t hash32_tuple(const H& h, const T& x) {
    return hash32_tuple<I + 1>(h.update(std::get<I>(x)), x);
}

template <int I,
          class H,
          class T,
          std::enable_if_t<(I == std::tuple_size<T>::value)>* = nullptr>
uint32_t hash32_tuple(const H& h, const T&) {
    return h.digest();
}

template <class... Args> uint32_t hash32(const std::tuple<Args...>& x) {
    return hash32_tuple(Hasher32<>{}, x);
}

}  // namespace internal

template <class T> struct UniversalHash32 {
    uint32_t operator()(const T& x) { return internal::hash32(x); }
};

}  // namespace yosupo
