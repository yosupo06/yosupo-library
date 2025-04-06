#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <ranges>
#include <vector>

#include "yosupo/algebra.hpp"
namespace yosupo {

template <monoid M> struct SegTree {
    using S = typename M::S;

  public:
    explicit SegTree(int n, M _m = M())
        : SegTree(std::vector<S>(n, _m.e), _m) {}

    explicit SegTree(const std::vector<S>& v, M _m = M())
        : SegTree(std::views::all(v), _m) {}

    template <std::ranges::forward_range R>
        requires std::ranges::sized_range<R> &&
                     std::convertible_to<std::ranges::range_value_t<R>, S>
    explicit SegTree(R&& r, M _m = M()) : m(_m), _n(int(std::ranges::size(r))) {
        size = (int)std::bit_ceil((unsigned int)(_n));
        log = std::countr_zero((unsigned int)size);
        d = std::vector<S>(2 * size, m.e);
        std::ranges::copy(r, d.begin() + size);
        for (int k = size - 1; k >= 1; k--) {
            update(k);
        }
    }

    void set(int p, S x) {
        assert(0 <= p && p < _n);
        p += size;
        d[p] = x;
        for (int i = 1; i <= log; i++) update(p >> i);
    }

    S get(int p) const {
        assert(0 <= p && p < _n);
        return d[p + size];
    }

    S prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        S sml = m.e, smr = m.e;
        l += size;
        r += size;

        while (l < r) {
            if (l & 1) sml = m.op(sml, d[l++]);
            if (r & 1) smr = m.op(d[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return m.op(sml, smr);
    }

    S all_prod() const { return d[1]; }

    template <bool (*f)(S)> int max_right(int l) const {
        return max_right(l, [](S x) { return f(x); });
    }
    template <class F> int max_right(int l, F f) const {
        assert(0 <= l && l <= _n);
        assert(f(m.e));
        if (l == _n) return _n;
        l += size;
        S sm = m.e;
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(op(sm, d[l]))) {
                while (l < size) {
                    l = (2 * l);
                    if (f(m.op(sm, d[l]))) {
                        sm = m.op(sm, d[l]);
                        l++;
                    }
                }
                return l - size;
            }
            sm = m.op(sm, d[l]);
            l++;
        } while ((l & -l) != l);
        return _n;
    }

    template <bool (*f)(S)> int min_left(int r) const {
        return min_left(r, [](S x) { return f(x); });
    }
    template <class F> int min_left(int r, F f) const {
        assert(0 <= r && r <= _n);
        assert(f(m.e));
        if (r == 0) return 0;
        r += size;
        S sm = m.e;
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(m.op(d[r], sm))) {
                while (r < size) {
                    r = (2 * r + 1);
                    if (f(m.op(d[r], sm))) {
                        sm = m.op(d[r], sm);
                        r--;
                    }
                }
                return r + 1 - size;
            }
            sm = m.op(d[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

  private:
    M m;
    int _n, size, log;
    std::vector<S> d;

    void update(int k) { d[k] = m.op(d[2 * k], d[2 * k + 1]); }
};

}  // namespace yosupo
