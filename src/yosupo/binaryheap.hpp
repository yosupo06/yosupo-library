#pragma once

#include <algorithm>
#include <functional>
#include <vector>

namespace yosupo {

template <class T, class Comp = std::less<T>> struct MeldableBinaryHeapManager {
    Comp comp;
    MeldableBinaryHeapManager(const Comp& _comp = Comp()) : comp(_comp) {}

    struct Heap {
        std::vector<T> d;
        size_t size() const { return std::size(d); }
        bool empty() const { return size() == 0; }
        void clear() { d.clear(); }
    };

    Heap build() { return Heap(); }

    Heap build(std::vector<T> d) {
        ssize_t n = std::ssize(d);
        auto h = Heap{std::move(d)};
        for (ssize_t i = n / 2 - 1; i >= 0; i--) {
            down(h, i);
        }
        return h;
    }

    void push(Heap& h, const T& x) {
        ssize_t c = std::ssize(h);
        h.d.push_back(x);
        while (c > 0) {
            ssize_t p = (c - 1) / 2;
            if (!bool(comp(h.d[p], h.d[c]))) break;
            std::swap(h.d[p], h.d[c]);
            c = p;
        }
    }

    void pop(Heap& h) {
        std::swap(h.d.front(), h.d.back());
        h.d.pop_back();
        if (!h.d.empty()) down(h, 0);
    }

    void pop_push(Heap& h, const T& x) {
        h.d.front() = x;
        down(h, 0);
    }

    const T& top(Heap& h) { return h.d.front(); }

    void meld(Heap& h, Heap& other) {
        if (h.size() < other.size()) std::swap(h, other);
        if (other.empty()) return;

        static constexpr ssize_t THRESHOLD = 50;
        if (std::ssize(other) < THRESHOLD) {
            for (auto x : other.d) {
                push(h, x);
            }
        } else {
            ssize_t l = std::ssize(h), r = l + std::ssize(other) - 1;

            h.d.insert(h.d.end(), other.d.begin(), other.d.end());

            while (l) {
                l = (l - 1) / 2;
                r = (r - 1) / 2;
                for (ssize_t i = r; i >= l; i--) {
                    down(h, i);
                }
            }
        }
        other = build();
    }

    size_t size(const Heap& h) const { return h.size(); }

  private:
    void down(Heap& h, ssize_t u) {
        ssize_t n = std::ssize(h);
        while (2 * u + 1 < n) {
            ssize_t v = (2 * u + 1);
            if (v + 1 < n && comp(h.d[v], h.d[v + 1])) v++;
            if (!comp(h.d[u], h.d[v])) break;
            std::swap(h.d[u], h.d[v]);
            u = v;
        }
    }
};

}  // namespace yosupo
