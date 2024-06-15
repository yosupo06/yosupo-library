#pragma once

#include <array>
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <random>
#include <type_traits>

namespace yosupo {

struct Xoshiro256StarStar {
  public:
    using result_type = uint64_t;
    Xoshiro256StarStar() : Xoshiro256StarStar(0) {}
    explicit Xoshiro256StarStar(uint64_t seed) {
        // use splitmix64
        // Reference: http://xoshiro.di.unimi.it/splitmix64.c
        for (int i = 0; i < 4; i++) {
            uint64_t z = (seed += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            s[i] = z ^ (z >> 31);
        }
    }

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return -1; }

    result_type operator()() {
        const uint64_t result_starstar = rotl(s[1] * 5, 7) * 9;

        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result_starstar;
    }

  private:
    // Use xoshiro256**
    // Refereces: http://xoshiro.di.unimi.it/xoshiro256starstar.c
    static uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    std::array<uint64_t, 4> s;
};

// https://github.com/wangyi-fudan/wyhash
struct WYRand {
  public:
    using result_type = uint64_t;
    explicit WYRand(uint64_t seed) : s(seed) {}

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return -1; }

    result_type operator()() {
        s += 0x2d358dccaa6c78a5;
        auto x = (unsigned __int128)s * (s ^ 0x8bb84b93962eacc9);
        return (uint64_t)(x ^ (x >> 64));
    }

  private:
    uint64_t s;
};
using Random = WYRand;
inline Random& global_gen() {
    static Random gen(
        std::chrono::steady_clock::now().time_since_epoch().count());
    return gen;
}

template <class G>
concept random_64 = std::uniform_random_bit_generator<G> &&
                    std::same_as<uint64_t, std::invoke_result_t<G&>> &&
                    G::min() == uint64_t(0) && G::max() == uint64_t(-1);

namespace internal {

// random choice from [0, upper]
template <random_64 G> uint64_t uniform_u64(uint64_t upper, G& gen) {
    if (!(upper & (upper + 1))) {
        // b = 00..0011..11
        return gen() & upper;
    }
    int log = 63 - std::countl_zero(upper);
    uint64_t mask = (log == 63) ? ~0ULL : (1ULL << (log + 1)) - 1;
    while (true) {
        uint64_t r = gen() & mask;
        if (r <= upper) return r;
    }
}

// random choice from [0, upper], faster than uniform_u64
template <random_64 G> uint64_t random_u64(uint64_t upper, G& gen) {
    return (uint64_t)(((unsigned __int128)(upper) + 1) * gen() >> 64);
}

}  // namespace internal

template <class T, random_64 G> T uniform(T lower, T upper, G& gen) {
    return T(lower +
             internal::uniform_u64(uint64_t(upper) - uint64_t(lower), gen));
}
template <class T> T uniform(T lower, T upper) {
    return uniform(lower, upper, global_gen());
}
template <class T, random_64 G> T random(T lower, T upper, G& gen) {
    return T(lower +
             internal::random_u64(uint64_t(upper) - uint64_t(lower), gen));
}
template <class T> T random(T lower, T upper) {
    return random(lower, upper, global_gen());
}

template <random_64 G> bool uniform_bool(G& gen) { return gen() & 1; }
inline bool uniform_bool() { return uniform_bool(global_gen()); }

// select 2 elements from [lower, uppper]
template <class T, random_64 G>
std::pair<T, T> uniform_pair(T lower, T upper, G& gen) {
    assert(upper - lower >= 1);
    T a, b;
    do {
        a = uniform(lower, upper, gen);
        b = uniform(lower, upper, gen);
    } while (a == b);
    if (a > b) std::swap(a, b);
    return {a, b};
}
template <class T> std::pair<T, T> uniform_pair(T lower, T upper) {
    return uniform_pair(lower, upper, global_gen());
}

// random value in the interval (0.0, 1.0]
template <class G> inline double open_closed_01(G& gen) {
    union {
        uint64_t i;
        double f;
    } u = {0xbff0000000000000 | (gen() >> 12)};
    return 2.0 + u.f;
}
inline double open_closed_01() {
    return open_closed_01(global_gen());
}

}  // namespace yosupo
