#pragma once

#include <immintrin.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <span>
#include <vector>

#include "yosupo/math/primitive_root.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/modint8.hpp"
#include "yosupo/types.hpp"
#include "yosupo/util.hpp"

namespace yosupo {

template <i32 MOD> struct FFTInfo {
    using modint = ModInt<MOD>;
    using modint8 = ModInt8<MOD>;

    static constexpr u32 g = smallest_primitive_root(MOD);
    static constexpr int ord2 = std::countr_zero((u32)(MOD - 1));

    // w[i]^(2^i) == 1 : w[i] is the fft omega of n=2^i
    static constexpr std::array<modint, ord2 + 1> w = []() {
        std::array<modint, ord2 + 1> v;
        v[ord2] = modint(g).pow((MOD - 1) >> ord2);
        for (int i = ord2 - 1; i >= 0; i--) {
            v[i] = v[i + 1] * v[i + 1];
        }
        return v;
    }();
    static constexpr std::array<modint, ord2 + 1> iw = []() {
        std::array<modint, ord2 + 1> v;
        v[ord2] = w[ord2].inv();
        for (int i = ord2 - 1; i >= 0; i--) {
            v[i] = v[i + 1] * v[i + 1];
        }
        return v;
    }();

    static constexpr std::array<modint, ord2 + 1> rot8 = []() {
        std::array<modint, std::max(0, ord2 + 1)> v;
        for (int i = 3; i <= ord2; i++) {
            v[i] = w[i];
            for (int j = 3; j < i; j++) {
                v[i] *= iw[j];
            }
        }
        return v;
    }();
    static constexpr std::array<modint, ord2 + 1> irot8 = []() {
        std::array<modint, std::max(0, ord2 + 1)> v;
        for (int i = 3; i <= ord2; i++) {
            v[i] = iw[i];
            for (int j = 3; j < i; j++) {
                v[i] *= w[j];
            }
        }
        return v;
    }();
    // rot[i] * rot_shift8(i) = rot[i + 8]
    modint rot_shift8(u32 i) const { return rot8[std::countr_one(i >> 3) + 3]; }
    modint irot_shift8(u32 i) const {
        return irot8[std::countr_one(i >> 3) + 3];
    }

    static constexpr std::array<modint, ord2 + 1> rot4 = []() {
        std::array<modint, std::max(0, ord2 + 1)> v;
        for (int i = 4; i <= ord2; i++) {
            v[i] = w[i];
            for (int j = 4; j < i; j++) {
                v[i] *= iw[j];
            }
        }
        return v;
    }();
    static constexpr std::array<modint, ord2 + 1> irot4 = []() {
        std::array<modint, std::max(0, ord2 + 1)> v;
        for (int i = 4; i <= ord2; i++) {
            v[i] = iw[i];
            for (int j = 4; j < i; j++) {
                v[i] *= w[j];
            }
        }
        return v;
    }();
    std::array<std::array<modint, 8>, ord2 + 1> rot16i = []() {
        std::array<std::array<modint, 8>, ord2 + 1> v;
        for (int i = 4; i <= ord2; i++) {
            std::array<modint, 8> buf;
            buf[0] = 1;
            for (int j = 1; j < 8; j++) {
                buf[j] = buf[j - 1] * rot4[i];
            }
            v[i] = buf;
        }
        return v;
    }();
    std::array<std::array<modint, 8>, ord2 + 1> irot16i = []() {
        std::array<std::array<modint, 8>, ord2 + 1> v;
        for (int i = 4; i <= ord2; i++) {
            std::array<modint, 8> buf;
            buf[0] = 1;
            for (int j = 1; j < 8; j++) {
                buf[j] = buf[j - 1] * irot4[i];
            }
            v[i] = buf;
        }
        return v;
    }();
    // rot[i * j] * rot_shift16i(i)[j] = rot[(i + 8) * j]
    __attribute__((target("avx2"))) modint8 rot_shift16i(u32 i) const {
        return modint8(rot16i[std::countr_one(i >> 4) + 4]);
    }
    __attribute__((target("avx2"))) modint8 irot_shift16i(u32 i) const {
        return modint8(irot16i[std::countr_one(i >> 4) + 4]);
    }
};
template <i32 MOD> const FFTInfo<MOD> fft_info = FFTInfo<MOD>();

template <i32 MOD>
__attribute__((target("avx2"))) void butterfly(std::vector<ModInt<MOD>>& a) {
    const int n = int(a.size());
    const int lg = std::countr_zero((u32)n);
    assert(n == (1 << lg));

    using modint8 = ModInt8<MOD>;
    const FFTInfo<MOD>& info = fft_info<MOD>;

    if (n == 1) {
        return;
    }
    if (n == 2) {
        auto a0 = a[0], a1 = a[1];
        a[0] = a0 + a1;
        a[1] = a0 - a1;
        return;
    }
    if (n == 4) {
        auto a0 = a[0], a1 = a[1], a2 = a[2], a3 = a[3];
        auto x = (a1 - a3) * info.w[2];

        a[0] = (a0 + a2) + (a1 + a3);
        a[1] = (a0 + a2) - (a1 + a3);
        a[2] = (a0 - a2) + x;
        a[3] = (a0 - a2) - x;
        return;
    }

    int h = lg;
    if (h % 2 == 0) {
        // 2-base
        int len = n / 2;
        for (int i = 0; i < len; i += 8) {
            auto l = modint8(subspan<8>(std::span{a}, 0 * len + i));
            auto r = modint8(subspan<8>(std::span{a}, 1 * len + i));

            std::copy_n((l + r).to_array().data(), 8,
                        subspan<8>(std::span{a}, 0 * len + i).data());
            std::copy_n((l - r).to_array().data(), 8,
                        subspan<8>(std::span{a}, 1 * len + i).data());
        }
        h--;
    }

    while (h >= 5) {
        // 4-base
        const modint8 w2x = modint8(info.w[2]);

        modint8 rotx = modint8(1);
        for (int start = 0; start < n; start += (1 << h)) {
            const modint8 rot2x = rotx * rotx;
            const modint8 rot3x = rot2x * rotx;

            int len = 1 << (h - 2);
            for (int i = 0; i < len; i += 8) {
                auto a0 =
                    modint8(subspan<8>(std::span{a}, start + 0 * len + i));
                auto a1 =
                    modint8(subspan<8>(std::span{a}, start + 1 * len + i)) *
                    rotx;
                auto a2 =
                    modint8(subspan<8>(std::span{a}, start + 2 * len + i)) *
                    rot2x;
                auto a3 =
                    modint8(subspan<8>(std::span{a}, start + 3 * len + i)) *
                    rot3x;

                auto x = (a1 - a3) * w2x;

                std::copy_n(
                    (a0 + a2 + a1 + a3).to_array().data(), 8,
                    subspan<8>(std::span{a}, start + 0 * len + i).data());
                std::copy_n(
                    (a0 + a2 - a1 - a3).to_array().data(), 8,
                    subspan<8>(std::span{a}, start + 1 * len + i).data());
                std::copy_n(
                    (a0 - a2 + x).to_array().data(), 8,
                    subspan<8>(std::span{a}, start + 2 * len + i).data());
                std::copy_n(
                    (a0 - a2 - x).to_array().data(), 8,
                    subspan<8>(std::span{a}, start + 3 * len + i).data());
            }
            rotx *= modint8(info.rot_shift8(8 * (start >> h)));
        }
        h -= 2;
    }

    {
        // fft each element
        const ModInt<MOD> w4 = info.w[2], w8 = info.w[3];
        const auto step4 = ModInt8<MOD>(1, 1, 1, w4, 1, 1, 1, w4);
        const auto step8 =
            ModInt8<MOD>(1, 1, 1, 1, 1, w8, w8 * w8, w8 * w8 * w8);

        modint8 rotxi = modint8(1);
        for (int i = 0; i < n; i += 8) {
            auto x = modint8(subspan<8>(std::span{a}, i)) * rotxi;
            x = (blend<0b11110000>(x, -x) +
                 x.permutevar({4, 5, 6, 7, 0, 1, 2, 3})) *
                step8;
            x = (blend<0b11001100>(x, -x) +
                 x.permutevar({2, 3, 0, 1, 6, 7, 4, 5})) *
                step4;
            x = (blend<0b10101010>(x, -x) +
                 x.permutevar({1, 0, 3, 2, 5, 4, 7, 6}));
            std::copy_n(x.to_array().data(), 8,
                        subspan<8>(std::span{a}, i).data());

            if (i + 8 < n) rotxi *= info.rot_shift16i(2 * i);
        }
    }
}

template <i32 MOD>
__attribute__((target("avx2"))) void butterfly_inv(
    std::vector<ModInt<MOD>>& a) {
    const int n = int(a.size());
    const int lg = std::countr_zero((u32)n);
    assert(n == (1 << lg));

    using modint8 = ModInt8<MOD>;
    const FFTInfo<MOD>& info = fft_info<MOD>;

    if (n == 1) {
        return;
    }
    if (n == 2) {
        auto a0 = a[0], a1 = a[1];
        a[0] = a0 + a1;
        a[1] = a0 - a1;
        return;
    }
    if (n == 4) {
        auto a0 = a[0], a1 = a[1], a2 = a[2], a3 = a[3];
        auto x0 = a0 + a1;
        auto x1 = a0 - a1;
        auto x2 = a2 + a3;
        auto x3 = (a2 - a3) * info.iw[2];

        a[0] = x0 + x2;
        a[1] = x1 + x3;
        a[2] = x0 - x2;
        a[3] = x1 - x3;
        return;
    }

    {
        // ifft each element
        const ModInt<MOD> iw4 = info.iw[2], iw8 = info.iw[3];
        const auto step4 = ModInt8<MOD>(1, 1, 1, iw4, 1, 1, 1, iw4);
        const auto step8 =
            ModInt8<MOD>(1, 1, 1, 1, 1, iw8, iw8 * iw8, iw8 * iw8 * iw8);
        modint8 irotxi = modint8(1);
        for (int i = 0; i < n; i += 8) {
            auto x = modint8(subspan<8>(std::span{a}, i));
            x = (blend<0b10101010>(x, -x) +
                 x.permutevar({1, 0, 3, 2, 5, 4, 7, 6})) *
                step4;
            x = (blend<0b11001100>(x, -x) +
                 x.permutevar({2, 3, 0, 1, 6, 7, 4, 5})) *
                step8;
            x = (blend<0b11110000>(x, -x) +
                 x.permutevar({4, 5, 6, 7, 0, 1, 2, 3}));

            std::copy_n((x * irotxi).to_array().begin(), 8,
                        subspan<8>(std::span{a}, i).begin());

            if (i + 8 < n) irotxi *= info.irot_shift16i(2 * i);
        }
    }

    int h = 3;
    while (h + 2 <= lg) {
        h += 2;

        // 4-base
        const modint8 w2 = modint8(info.iw[2]);

        modint8 rotx = modint8(1);
        for (int start = 0; start < n; start += (1 << h)) {
            const auto rot2x = rotx * rotx;
            const auto rot3x = rot2x * rotx;
            int len = 1 << (h - 2);
            for (int i = 0; i < len; i += 8) {
                auto a0 =
                    modint8(subspan<8>(std::span{a}, start + 0 * len + i));
                auto a1 =
                    modint8(subspan<8>(std::span{a}, start + 1 * len + i));
                auto a2 =
                    modint8(subspan<8>(std::span{a}, start + 2 * len + i));
                auto a3 =
                    modint8(subspan<8>(std::span{a}, start + 3 * len + i));

                auto x0 = a0 + a1;
                auto x1 = a0 - a1;
                auto x2 = a2 + a3;
                auto x3 = (a2 - a3) * w2;

                std::copy_n(
                    (x0 + x2).to_array().begin(), 8,
                    subspan<8>(std::span{a}, start + 0 * len + i).begin());
                std::copy_n(
                    ((x1 + x3) * rotx).to_array().begin(), 8,
                    subspan<8>(std::span{a}, start + 1 * len + i).begin());
                std::copy_n(
                    ((x0 - x2) * rot2x).to_array().begin(), 8,
                    subspan<8>(std::span{a}, start + 2 * len + i).begin());
                std::copy_n(
                    ((x1 - x3) * rot3x).to_array().data(), 8,
                    subspan<8>(std::span{a}, start + 3 * len + i).begin());
            }
            rotx *= modint8(info.irot_shift8(8 * (start >> h)));
        }
    }

    if (h + 1 == lg) {
        // 2-base
        int len = n / 2;
        for (int i = 0; i < len; i += 8) {
            auto l = modint8(subspan<8>(std::span{a}, 0 * len + i));
            auto r = modint8(subspan<8>(std::span{a}, 1 * len + i));

            std::copy_n((l + r).to_array().begin(), 8,
                        subspan<8>(std::span{a}, 0 * len + i).begin());
            std::copy_n((l - r).to_array().begin(), 8,
                        subspan<8>(std::span{a}, 1 * len + i).begin());
        }
        h++;
    }
}

template <i32 MOD>
std::vector<ModInt<MOD>> convolution(std::vector<ModInt<MOD>> a,
                                     std::vector<ModInt<MOD>> b) {
    int n = int(a.size()), m = int(b.size());
    int z = (int)std::bit_ceil((unsigned int)(n + m - 1));

    a.resize(z);
    butterfly(a);
    b.resize(z);
    butterfly(b);
    for (int i = 0; i < z; i++) {
        a[i] *= b[i];
    }
    butterfly_inv(a);
    a.resize(n + m - 1);
    auto iz = ModInt<MOD>(z).inv();
    for (int i = 0; i < n + m - 1; i++) a[i] *= iz;
    return a;
}

}  // namespace yosupo
