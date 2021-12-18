#pragma once

#include "atcoder/internal_bit"
#include "atcoder/internal_math"
#include "yosupo/simd/modint.hpp"

#include <immintrin.h>
#include <array>
#include <vector>

namespace yosupo {

template <int MOD> struct fft_info {
    using mint = static_modint<MOD>;
    using mintx8 = modintx8<MOD>;
    static constexpr int g = atcoder::internal::primitive_root<MOD>;

    static constexpr int rank2 =
        atcoder::internal::bsf_constexpr(mint::mod() - 1);
    std::array<mint, rank2 + 1> root,
        iroot;  // root[i]^(2^i) == 1, root[i] * iroot[i] == 1

    std::array<mint, std::max(0, rank2 - 1 + 1)> rate2, irate2;
    std::array<mintx8, std::max(0, rank2 - 1 + 1)> rate2x;

    std::array<mint, std::max(0, rank2 - 2 + 1)> rate3, irate3;

    std::array<mint, std::max(0, rank2 - 3 + 1)> rate4, irate4;
    std::array<mintx8, std::max(0, rank2 - 3 + 1)> rate4xi,
        irate4xi;  // rate4xi[i][j] = rate4[i]^j

    fft_info() {
        root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
        iroot[rank2] = root[rank2].inv();
        for (int i = rank2 - 1; i >= 0; i--) {
            root[i] = root[i + 1] * root[i + 1];
            iroot[i] = iroot[i + 1] * iroot[i + 1];
        }

        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2[i] = root[i + 2] * prod;
                irate2[i] = iroot[i + 2] * iprod;
                prod *= iroot[i + 2];
                iprod *= root[i + 2];
            }
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2x[i] = mintx8(rate2[i]);
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 3; i++) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 4; i++) {
                rate4[i] = root[i + 4] * prod;
                irate4[i] = iroot[i + 4] * iprod;
                prod *= iroot[i + 4];
                iprod *= root[i + 4];
                std::array<mint, 8> buf, ibuf;
                for (int j = 0; j < 8; j++) {
                    buf[j] = rate4[i].pow(j);
                    ibuf[j] = irate4[i].pow(j);
                }
                rate4xi[i] = buf;
                irate4xi[i] = ibuf;
            }
        }
    }
};

template <int MOD> void butterfly(std::vector<modintx8<MOD>>& _a) {
    int n = int(_a.size() * 8);
    using mint = static_modint<MOD>;
    using mintx8 = modintx8<MOD>;

    int h = internal::ceil_pow2(n);

    static const fft_info<MOD> info;
    const mint imag = info.root[2];

    assert(n >= 8 && n % 8 == 0);
    int n8 = n / 8;

    int len = 0;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed

    if (h % 2 == 0) {
        // 2-base
        int p = n8 / 2;
        for (int i = 0; i < p; i++) {
            auto l = _a[i];
            auto r = _a[i + p];
            _a[i] = l + r;
            _a[i + p] = l - r;
        }
        len++;
    }

    while (len + 5 <= h) {
        // 4-base
        int p = 1 << (h - len - 5);
        mintx8 rotx(1);
        auto imagx = mintx8(imag);
        imagx.d -= intx8(998244353) & (imagx.d > intx8(998244352));
        for (int s = 0; s < (1 << len); s++) {
            auto rot2x = rotx * rotx;
            auto rot3x = rot2x * rotx;
            int offset = s << (h - len - 3);
            for (int i = 0; i < p; i++) {
                auto a0 = _a[i + offset + 0 * p];
                auto a1 = _a[i + offset + 1 * p] * rotx;
                auto a2 = _a[i + offset + 2 * p] * rot2x;
                auto a3 = _a[i + offset + 3 * p] * rot3x;
                _a[i + offset + 0 * p] = (a0 + a2) + (a1 + a3);
                _a[i + offset + 1 * p] = (a0 + a2) - (a1 + a3);
                _a[i + offset + 2 * p] = (a0 - a2) + (a1 - a3) * imagx;
                _a[i + offset + 3 * p] = (a0 - a2) - (a1 - a3) * imagx;
            }
            rotx *= mintx8(info.rate3[bsf(~(unsigned int)(s))]);
        }
        len += 2;
    }

    {
        // 8-base
        assert(len + 3 == h);
        mint e8 = info.root[3];
        const mintx8 step1 = mintx8(1, 1, 1, 1, 1, e8, e8 * e8, e8 * e8 * e8);
        const mintx8 step2 = mintx8(1, 1, 1, imag, 1, 1, 1, imag);
        auto rotxi = mintx8(1);
        for (int s = 0; s < n8; s++) {
            mintx8 v = _a[s] * rotxi;
            v = (v.template neg<0b11110000>() + v.template shufflex4<0b01>()) *
                step1;
            v = (v.template neg<0b11001100>() +
                 v.template shuffle<0b01001110>()) *
                step2;
            v = (v.template neg<0b10101010>() +
                 v.template shuffle<0b10110001>());
            _a[s] = v;
            rotxi *= info.rate4xi[bsf(~(unsigned int)(s))];
        }
        len += 3;
    }
}

template <int MOD> void butterfly_inv(std::vector<modintx8<MOD>>& _a) {
    int n = int(_a.size() * 8);
    using mint = static_modint<MOD>;
    using mintx8 = modintx8<MOD>;

    int h = internal::ceil_pow2(n);

    static const fft_info<MOD> info;

    assert(n >= 8 && n % 8 == 0);
    const mint iimag = info.iroot[2];
    const mintx8 iimagx = iimag;

    int n8 = n / 8;

    int len = h;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed

    {
        // 8-base
        mint ie8 = info.iroot[3];
        const mintx8 istep1 =
            mintx8(1, 1, 1, 1, 1, ie8, ie8 * ie8, ie8 * ie8 * ie8);
        const mintx8 istep2 = mintx8(1, 1, 1, iimag, 1, 1, 1, iimag);
        auto irotxi = mintx8(1);
        for (int s = 0; s < n8; s++) {
            auto v = _a[s];
            v = (v.template neg<0b10101010>() +
                 v.template shuffle<0b10110001>()) *
                istep2;
            v = (v.template neg<0b11001100>() +
                 v.template shuffle<0b01001110>()) *
                istep1;
            v = (v.template neg<0b11110000>() + v.template shufflex4<0b01>()) *
                irotxi;
            _a[s] = v;
            irotxi *= info.irate4xi[bsf(~(unsigned int)(s))];
        }
        len -= 3;
    }

    while (len >= 2) {
        int p = 1 << (h - len - 3);
        auto irotx = mintx8(1);
        for (int s = 0; s < (1 << (len - 2)); s++) {
            auto irot2x = irotx * irotx;
            auto irot3x = irot2x * irotx;
            int offset = s << (h - len - 1);
            for (int i = 0; i < p; i++) {
                auto a0 = _a[i + offset + 0 * p];
                auto a1 = _a[i + offset + 1 * p];
                auto a2 = _a[i + offset + 2 * p];
                auto a3 = _a[i + offset + 3 * p];
                auto a0a1 = a0 + a1;
                auto a0na1 = a0 - a1;
                auto a2a3 = a2 + a3;
                auto a2na3iimag = (a2 - a3) * iimagx;

                _a[i + offset + 0 * p] = a0a1 + a2a3;
                _a[i + offset + 1 * p] = (a0na1 + a2na3iimag) * irotx;
                _a[i + offset + 2 * p] = (a0a1 - a2a3) * irot2x;
                _a[i + offset + 3 * p] = (a0na1 - a2na3iimag) * irot3x;
            }
            irotx *= info.irate3[bsf(~(unsigned int)(s))];
        }
        len -= 2;
    }

    if (len == 1) {
        int p = 1 << (h - 4);
        for (int i = 0; i < p; i++) {
            auto l = _a[i];
            auto r = _a[i + p];
            _a[i] = l + r;
            _a[i + p] = l - r;
        }
        len--;
    }
}

template <int MOD>
std::vector<modintx8<MOD>> convolution(std::vector<modintx8<MOD>> a,
                                       std::vector<modintx8<MOD>> b) {
    int n = int(a.size());
    int m = int(b.size());
    int z = 1 << internal::ceil_pow2(n + m);
    a.resize(z);
    butterfly(a);
    b.resize(z);
    butterfly(b);
    for (int i = 0; i < z; i++) {
        a[i] *= b[i];
    }
    butterfly_inv(a);
    a.resize(n + m);
    modintx8<MOD> iz = static_modint<MOD>(8 * z).inv();
    for (int i = 0; i < n + m; i++) a[i] *= iz;
    return a;
}

template <int MOD> void butterfly(std::vector<static_modint<MOD>>& a) {
    using mint = static_modint<MOD>;
    using mintx8 = modintx8<MOD>;
    int n = int(a.size());
    int n2 = (n + 7) / 8;
    std::vector<mintx8> a2(n2);
    for (int i = 0; i < n2; i++) {
        std::array<mint, 8> v;
        for (int j = 0; j < 8 && (i * 8 + j) < n; j++) {
            v[j] = a[i * 8 + j];
        }
        a2[i] = v;
    }
    butterfly(a2);
    for (int i = 0; i < n2; i++) {
        auto v = a2[i].to_array();
        for (int j = 0; j < 8 && (i * 8 + j) < n; j++) {
            a[i * 8 + j] = v[j];
        }
    }
}
template <int MOD> void butterfly_inv(std::vector<static_modint<MOD>>& a) {
    using mint = static_modint<MOD>;
    using mintx8 = modintx8<MOD>;
    int n = int(a.size());
    int n2 = (n + 7) / 8;
    std::vector<mintx8> a2(n2);
    for (int i = 0; i < n2; i++) {
        std::array<mint, 8> v;
        for (int j = 0; j < 8 && (i * 8 + j) < n; j++) {
            v[j] = a[i * 8 + j];
        }
        a2[i] = v;
    }
    butterfly_inv(a2);
    for (int i = 0; i < n2; i++) {
        auto v = a2[i].to_array();
        for (int j = 0; j < 8 && (i * 8 + j) < n; j++) {
            a[i * 8 + j] = v[j];
        }
    }
}

}  // namespace yosupo
