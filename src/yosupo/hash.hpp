#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "yosupo/random.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

namespace internal {

// simple hash based on FNV-hash + wymix (of wyhash)
inline u64 wymix(u64 a, u64 b) {
    u128 c = (u128)a * b;
    return u64(c) ^ u64(c >> 64);
}
struct Hasher {
    u64 a, b;

    auto update64(u64 x) { a = wymix(a ^ x, MULTIPLE); }

    u64 digest() { return wymix(a, b); }

    template <std::integral T>
        requires(sizeof(T) <= 8)
    void update(const T& x) {
        update64(x);
    }

    template <class T>
        requires(std::same_as<T, i128> || std::same_as<T, u128>)
    void update(const T& x) {
        update64(u64(x));
        update64(u64((u128)(x) >> 64));
    }

    // pair
    template <class S, class T> void update(const std::pair<S, T>& x) {
        update(x.first);
        update(x.second);
    }

    // tuple
    template <class... Args> auto update(const std::tuple<Args...>& x) {
        std::apply([&](const auto&... y) { (update(y), ...); }, x);
    }

    // array
    template <class T, size_t N> auto update(const std::array<T, N>& x) {
        for (const auto& y : x) {
            update(y);
        }
    }

    // vector
    template <class T> auto update(const std::vector<T>& x) {
        update(x.size());
        for (const auto& y : x) {
            update(y);
        }
    }

    // string
    auto update(const std::string& x) {
        update(x.size());
        // TODO: should handle as 8-byte chunks
        for (const auto& y : x) {
            update(y);
        }
    }

    // set
    template <class T> auto update(const std::set<T>& x) {
        update(x.size());
        for (const auto& y : x) {
            update(y);
        }
    }

    // map
    template <class T, class U> auto update(const std::map<T, U>& x) {
        update(x.size());
        for (const auto& y : x) {
            update(y);
        }
    }

    const u64 MULTIPLE = 6364136223846793005;
};

}  // namespace internal

struct Hasher {
    inline static Random hash_gen = get_random();

    const u64 a = uniform<u64>(hash_gen);
    const u64 b = uniform<u64>(hash_gen);

    template <class T> u64 operator()(const T& x) const {
        internal::Hasher h{a, b};
        h.update(x);
        return h.digest();
    }
};

}  // namespace yosupo
