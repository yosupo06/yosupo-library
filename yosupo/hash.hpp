#pragma once

#include <array>
#include <cstdint>
#include <tuple>
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

struct DynHasher32 {
    int len = 0;
    uint64_t v = get_seed(0);

    DynHasher32 update32(uint32_t x) const {
        return DynHasher32{len + 1, v + x * get_seed(len)};
    }

    DynHasher32 to_dyn() const {
        return *this;
    }
    uint32_t digest() const { return uint32_t(v >> 32); }
};

template <int I = 1> struct Hasher32 {
    uint64_t v = get_seed(0);

    Hasher32<I + 1> update32(uint32_t x) const {
        return Hasher32<I + 1>{v + x * get_seed(I)};
    }

    DynHasher32 to_dyn() const { return DynHasher32{I, v}; }

    uint32_t digest() const { return uint32_t(v >> 32); }
};


template <class H, class T,
          is_integral_t<T>* = nullptr,
          std::enable_if_t<sizeof(T) <= 4>* = nullptr>
auto update(const H& h, const T& x) {
    return h.update32(uint32_t(x));
}

template <class H,
          class T,
          is_integral_t<T>* = nullptr,
          std::enable_if_t<sizeof(T) == 8>* = nullptr>
auto update(const H& h, const T& x) {
    return update(update(h, uint32_t(x)), uint32_t(uint64_t(x) >> 32));
}

template <class H,
          class T,
          is_integral_t<T>* = nullptr,
          std::enable_if_t<sizeof(T) == 16>* = nullptr>
auto update(const H& h, const T& x) {
    return update(update(h, uint64_t(x)), uint64_t((__uint128_t)(x) >> 64));
}

template <class H, class S, class T>
auto update(const H& h, const std::pair<S, T>& x) {
    return update(update(h, x.first), x.second);
}

template <int I,
          class H,
          class T,
          std::enable_if_t<(I == std::tuple_size<T>::value)>* = nullptr>
auto update_tuple(const H& h, const T&) {
    return h;
}

template <int I = 0,
          class H,
          class T,
          std::enable_if_t<(I != std::tuple_size<T>::value)>* = nullptr>
auto update_tuple(const H& h, const T& x) {
    return update(h, std::get<I>(x));
}


template <class H, class... Args>
auto update(const H& h, const std::tuple<Args...>& x) {
    return update_tuple(h, x);
}

template <class... Args> uint32_t hash32(const std::tuple<Args...>& x) {
    return hash32_tuple(Hasher32<>{}, x);
}

}  // namespace internal

template <class T> struct UniversalHash32 {
    uint32_t operator()(const T& x) { return update(internal::Hasher32<>{}, x).digest(); }
};

}  // namespace yosupo
