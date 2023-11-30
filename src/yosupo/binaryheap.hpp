#include <algorithm>
#include <functional>
#include <vector>

namespace yosupo {

template <class T, class Comp = std::less<T>> struct MeldableBinaryHeapManager {
    Comp comp;
    MeldableBinaryHeapManager(const Comp& _comp = Comp()) : comp(_comp) {}

    struct Heap {
        std::vector<T> d;
        int len() const { return int(d.size()); }
        bool empty() const { return len() == 0; }
        void clear() { d.clear(); }
    };

    Heap build() { return Heap(); }

    Heap build(std::vector<T> d) {
        int n = int(d.size());
        auto h = Heap{std::move(d)};
        for (int i = n / 2 - 1; i >= 0; i--) {
            down(h, i);
        }
        return h;
    }

    void push(Heap& h, const T& x) {
        int c = int(h.d.size());
        h.d.push_back(x);
        while (c > 0) {
            int p = (c - 1) / 2;
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

    const T& top(Heap& h) { return h.d.front(); }

    void meld(Heap& h, Heap& other) {
        if (h.len() < other.len()) swap(h, other);
        if (other.empty()) return;

        static constexpr int THRESHOLD = 50;
        if (other.len() < THRESHOLD) {
            for (auto x : other.d) {
                push(h, x);
            }
        } else {
            int l = h.len(), r = l + other.len() - 1;

            h.d.insert(h.d.end(), other.d.begin(), other.d.end());

            while (l) {
                l = (l - 1) / 2;
                r = (r - 1) / 2;
                for (int i = r; i >= l; i--) {
                    down(h, i);
                }
            }
        }
        other = build();
    }

    int len(const Heap& h) const { return int(h.d.size()); }

  private:
    void down(Heap& h, int u) {
        int n = int(h.d.size());
        while (2 * u + 1 < n) {
            int v = (2 * u + 1);
            if (v + 1 < n && comp(h.d[v], h.d[v + 1])) v++;
            if (!comp(h.d[u], h.d[v])) break;
            std::swap(h.d[u], h.d[v]);
            u = v;
        }
    }
};

}  // namespace yosupo
