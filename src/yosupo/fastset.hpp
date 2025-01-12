#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <vector>

namespace yosupo {

struct FastSet {
    static constexpr int B = 64;

    // bool配列a_0, a_1, ..., a_nを作る
    explicit FastSet(int n) : _n(n) {
        do {
            seg.push_back(std::vector<uint64_t>((n + B - 1) / B));
            n = (n + B - 1) / B;
        } while (n > 1);
        _log = int(seg.size());
    }
    // bool配列a_0, a_1, ..., a_nを作る
    template <class F> explicit FastSet(int n, F f) : _n(n) {
        {
            int n2 = (n + B - 1) / B;
            seg.push_back(std::vector<uint64_t>(n2));
            for (int i = 0; i < n; i++) {
                if (f(i)) seg[0][i / B] |= 1ULL << (i % B);
            }
            n = n2;
        }
        while (n > 1) {
            int n2 = (n + B - 1) / B;
            std::vector<uint64_t> v2(n2);
            for (int i = 0; i < n; i++) {
                if (seg.back()[i]) {
                    v2[i / B] |= 1ULL << (i % B);
                }
            }
            seg.push_back(std::move(v2));
            n = n2;
        }
        _log = int(seg.size());
    }

    size_t size() const { return _n; }

    // a[i]を返す。a[i] = falseは出来ないことに注意(要検討)
    bool operator[](int i) const { return (seg[0][i / B] >> (i % B) & 1) != 0; }

    // a[i] = true
    void set(int i) {
        for (int h = 0; h < _log; h++) {
            seg[h][i / B] |= 1ULL << (i % B);
            i /= B;
        }
    }

    // a[i] = false
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
            uint64_t d = seg[h][i / B] >> (i % B);
            if (!d) {
                i = i / B + 1;
                continue;
            }
            // find
            i += std::countr_zero(d);
            for (int g = h - 1; g >= 0; g--) {
                i *= B;
                i += std::countr_zero(seg[g][i / B]);
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
            uint64_t d = seg[h][i / B] << (63 - i % 64);
            if (!d) {
                i = i / B - 1;
                continue;
            }
            // find
            i -= std::countl_zero(d);
            for (int g = h - 1; g >= 0; g--) {
                i *= B;
                i += B - 1 - std::countl_zero(seg[g][i / B]);
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
    std::vector<std::vector<uint64_t>> seg;
};

}  // namespace yosupo
