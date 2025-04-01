#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "yosupo/container/bitvector.hpp"
#include "yosupo/random.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

namespace internal {

constexpr int THRESHOLD_DYN = 100;

inline Random hash_gen = get_random();
inline std::array<u64, THRESHOLD_DYN> seeds = []() {
    std::array<u64, THRESHOLD_DYN> _seeds;
    for (auto& x : _seeds) {
        x = yosupo::uniform<u64>(hash_gen);
    }
    return _seeds;
}();

// wyhash like
const u64 secret0 = 0x2d358dccaa6c78a5;
const u64 secret1 = 0x8bb84b93962eacc9;
inline u64 hash64(u64 x, u64 seed) {
    u128 a = (u128)(x)*seed;
    u64 a0 = u64(a), a1 = u64(a >> 64);
    a0 ^= secret0;
    a1 ^= secret1;
    u128 b = (u128)a0 * a1;
    return u64(b) ^ u64(b >> 64);
}

struct DynHasher {
    Random gen;
    u64 v;

    DynHasher(u64 seed) : gen(Random(seed)), v(seed) {}
    DynHasher(Random _gen, u64 _v) : gen(_gen), v(_v) {}

    DynHasher update(u64 x) && {
        u64 seed = gen();
        v ^= hash64(x, seed);
        return *this;
    }

    DynHasher to_dyn() && { return *this; }
    u64 digest() { return v; }
};

template <int I> struct Hasher {
    u64 v;
    auto update(u64 x) && {
        if constexpr (I == THRESHOLD_DYN) {
            return Hasher<I>{v}.to_dyn().update(x);
        } else {
            return Hasher<I + 1>{v ^ hash64(x, seeds[I])};
        }
    }

    DynHasher to_dyn() && { return DynHasher(digest()); }
    u64 digest() { return v; }
};

inline auto get_hasher() { return internal::Hasher<0>{0}; }

// integer
template <class H, std::integral T>
    requires(sizeof(T) <= 8)
auto update(H h, const T& x) {
    return std::move(h).update(u64(x));
}

template <class H, class T>
    requires(std::same_as<T, i128> || std::same_as<T, u128>)
auto update(H h, const T& x) {
    return update(update(std::move(h), u64(x)), u64((u128)(x) >> 64));
}

// pair
template <class H, class S, class T>
auto update(H h, const std::pair<S, T>& x) {
    return update(update(std::move(h), x.first), x.second);
}

// long array
template <size_t I = 0, class H, class T, size_t N>
    requires(N < THRESHOLD_DYN)
auto update(H h, const std::array<T, N>& x) {
    if constexpr (I == N) {
        return h;
    } else {
        return update<I + 1>(update(std::move(h), std::get<I>(x)), x);
    }
}

// long array
template <class H, class T, size_t N>
    requires(N >= THRESHOLD_DYN)
auto update(H h, const std::array<T, N>& x) {
    auto h2 = std::move(h).to_dyn();
    for (const auto& y : x) {
        h2 = update(std::move(h2), y);
    }
    return h2;
}

// tuple
template <int I = 0, class H, class... Args>
auto update(H h, const std::tuple<Args...>& x) {
    if constexpr (I == std::tuple_size<std::tuple<Args...>>::value) {
        return h;
    } else {
        return update<I + 1>(update(std::move(h), std::get<I>(x)), x);
    }
}

// vector
template <class H, class T> auto update(H h, const std::vector<T>& v) {
    auto h2 = update(h, v.size()).to_dyn();
    for (const auto& x : v) {
        h2 = update(std::move(h2), x);
    }
    return h2;
}

// set
template <class H, class T> auto update(H h, const std::set<T>& s) {
    auto h2 = update(std::move(h), s.size()).to_dyn();
    for (const auto& x : s) {
        h2 = update(std::move(h2), x);
    }
    return h2;
}

// map
template <class H, class T, class U>
auto update(const H& h, const std::map<T, U>& m) {
    auto h2 = update(std::move(h), m.size()).to_dyn();
    for (const auto& x : m) {
        h2 = update(std::move(h2), x);
    }
    return h2;
}

}  // namespace internal

template <class T> struct Hasher {
    u64 operator()(const T& x) {
        return internal::update(internal::get_hasher(), x).digest();
    }
};

}  // namespace yosupo
