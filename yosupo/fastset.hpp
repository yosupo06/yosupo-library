#pragma once

#include <algorithm>
#include <vector>
#include <yosupo/internal_bit>

namespace yosupo {

struct FastSet {
    static constexpr int B = 64;

    FastSet(int n) : _n(n) {
        do {
            seg.push_back(std::vector<unsigned long long>((n + B - 1) / B));
            n = (n + B - 1) / B;
        } while (n > 1);
        _log = int(seg.size());
    }
    bool operator[](int i) const { return (seg[0][i / B] >> (i % B) & 1) != 0; }
    void set(int i) {
        for (int h = 0; h < _log; h++) {
            seg[h][i / B] |= 1ULL << (i % B);
            i /= B;
        }
    }
    void reset(int i) {
        for (int h = 0; h < _log; h++) {
            seg[h][i / B] &= ~(1ULL << (i % B));
            if (seg[h][i / B]) break;
            i /= B;
        }
    }

    // @return i以上最小の要素 or n
    int or_more(int i) const {
        if (i >= _n) return _n;
        i = std::max(i, 0);
        for (int h = 0; h < _log; h++) {
            if (i / B == int(seg[h].size())) break;
            unsigned long long d = seg[h][i / B] >> (i % B);
            if (!d) {
                i = i / B + 1;
                continue;
            }
            // find
            i += internal::bsf(d);
            for (int g = h - 1; g >= 0; g--) {
                i *= B;
                i += internal::bsf(seg[g][i / B]);
            }
            assert((*this)[i]);
            return i;
        }
        return _n;
    }

    // @return iより大きい最小の要素 or n
    int more(int i) const {
        if (i >= _n - 1) return _n;
        return or_more(i + 1);
    }

    // @return i以下最大の要素 or -1
    int or_less(int i) const {
        if (i < 0) return -1;
        i = std::min(i, _n - 1);
        for (int h = 0; h < _log; h++) {
            if (i == -1) break;
            unsigned long long d = seg[h][i / B] << (63 - i % 64);
            if (!d) {
                i = i / B - 1;
                continue;
            }
            // find
            i += internal::bsr(d) - (B - 1);
            for (int g = h - 1; g >= 0; g--) {
                i *= B;
                i += internal::bsr(seg[g][i / B]);
            }
            return i;
        }
        return -1;
    }

    // @return iより小さい最大の要素 or -1
    int less(int i) const {
        if (i <= 0) return -1;
        return or_less(i - 1);
    }

  private:
    int _n, _log;
    std::vector<std::vector<unsigned long long>> seg;
};

}  // namespace yosupo
