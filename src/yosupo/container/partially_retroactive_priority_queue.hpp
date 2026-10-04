#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace yosupo {

// A fixed timeline of n operations, initially all no-ops. Each update replaces
// one operation at 0 <= t < n. Empty pops are ignored; failed_pops() counts
// them.
//
// Compare orders elements from highest to lowest priority (std::less<T> gives
// a min-queue). Equivalent values are ordered by their push time, earlier
// first. T must be copyable; no default value, numeric sentinel, or equality is
// needed.
//
// Each update is O(log(n + 1)); queries are O(1).
// Construction and space are O(n + 1).
// Change describes the final queue, not the elements popped along the timeline.
// Apply removed before added: replacing a push can put its time in both lists,
// with the old and new values respectively. There are at most two entries
// total.
template <class T, class Compare = std::less<T>>
struct PartiallyRetroactivePriorityQueue {
    struct Entry {
        int time;
        T value;
    };

    struct Change {
        std::vector<Entry> added;
        std::vector<Entry> removed;
    };

    explicit PartiallyRetroactivePriorityQueue(int n,
                                               const Compare& comp = Compare())
        : _n(n), _comp(comp) {
        assert(n >= 0);
        values.resize(n);
        while (capacity < 2 * n) capacity *= 2;
        nodes.resize(2 * capacity);
        // n virtual pushes precede the real timeline. They have lower priority
        // than every real value, so every pop can succeed internally. Popping
        // a virtual element corresponds exactly to an ignored empty pop.
        for (int i = 0; i < n; i++) nodes[capacity + i].live = i;
        for (int i = capacity - 1; i > 0; i--) pull(i);
    }

    Change set_push(int t, T value) {
        assert(0 <= t && t < _n);
        Change change;
        erase_operation(t, change);
        values[t] = std::move(value);
        int p = _n + t;
        int b = last_bridge(1, 0, capacity, p, 0);
        int candidate = prod(b, 2 * _n).dead;
        if (candidate != -1 && less(p, candidate)) {
            set(p, Node{1, 0, -1, p});
            set(candidate, Node{0, 0, candidate, -1});
            add_entry(candidate, change);
        } else {
            set(p, Node{0, 0, p, -1});
            add_entry(p, change);
        }
        push_count++;
        cancel_unchanged(change, t);
        return change;
    }

    Change set_pop(int t) {
        assert(0 <= t && t < _n);
        Change change;
        erase_operation(t, change);
        int p = _n + t;
        int b = first_bridge(1, 0, capacity, p, 0);
        int candidate = prod(0, b).live;
        assert(candidate != -1);
        remove_entry(candidate, change);
        set(candidate, Node{1, 0, -1, candidate});
        set(p, Node{-1, -1, -1, -1});
        pop_count++;
        cancel_unchanged(change, t);
        return change;
    }

    // Replace op[t] with a no-op.
    Change clear(int t) {
        assert(0 <= t && t < _n);
        Change change;
        erase_operation(t, change);
        return change;
    }

    int size() const { return _size; }
    bool empty() const { return _size == 0; }
    int failed_pops() const { return _size - push_count + pop_count; }

    // The minimum under Compare. Requires !empty().
    const T& min() const {
        assert(!empty());
        return *values[nodes[1].live - _n];
    }

  private:
    // Live pushes are in the final queue; dead pushes were popped. Assign +1
    // to dead pushes, -1 to pops, and 0 to everything else. Prefix balances
    // are nonnegative and end at 0. A zero-balance boundary is a "bridge".
    // A new push competes with the largest dead push after the preceding
    // bridge; the larger one becomes live. Removing a pop revives that same
    // candidate. Adding a pop / removing a dead push kills the smallest live
    // push before the following bridge. Only one final membership can change.
    struct Node {
        int sum = 0;
        int min_prefix = 0;
        int live = -1;
        int dead = -1;
    };

    int _n;
    Compare _comp;
    int capacity = 1;
    int _size = 0, push_count = 0, pop_count = 0;
    std::vector<std::optional<T>> values;
    std::vector<Node> nodes;

    bool less(int a, int b) const {
        if (a < _n || b < _n) {
            if ((a < _n) != (b < _n)) return b < _n;
        } else {
            if (_comp(*values[a - _n], *values[b - _n])) return true;
            if (_comp(*values[b - _n], *values[a - _n])) return false;
        }
        return a < b;
    }

    Node op(const Node& a, const Node& b) const {
        Node res;
        res.sum = a.sum + b.sum;
        res.min_prefix = std::min(a.min_prefix, a.sum + b.min_prefix);
        res.live = a.live;
        if (b.live != -1 && (res.live == -1 || less(b.live, res.live))) {
            res.live = b.live;
        }
        res.dead = a.dead;
        if (b.dead != -1 && (res.dead == -1 || less(res.dead, b.dead))) {
            res.dead = b.dead;
        }
        return res;
    }

    void pull(int p) { nodes[p] = op(nodes[2 * p], nodes[2 * p + 1]); }

    void set(int p, const Node& node) {
        p += capacity;
        nodes[p] = node;
        while ((p /= 2) > 0) pull(p);
    }

    Node prod(int l, int r) const {
        Node left, right;
        l += capacity;
        r += capacity;
        while (l < r) {
            if (l & 1) left = op(left, nodes[l++]);
            if (r & 1) right = op(nodes[--r], right);
            l /= 2;
            r /= 2;
        }
        return op(left, right);
    }

    // Find a zero prefix balance at a boundary <= t / >= t. Subtree prefix
    // minima let each search skip intervals without bridges in O(log n) time.
    int last_bridge(int p, int l, int r, int t, int before) const {
        if (l > t || before + nodes[p].min_prefix > 0) return -1;
        if (r - l == 1) {
            if (r <= t && before + nodes[p].sum == 0) return r;
            return before == 0 ? l : -1;
        }
        int m = (l + r) / 2;
        int res = last_bridge(2 * p + 1, m, r, t, before + nodes[2 * p].sum);
        if (res != -1) return res;
        return last_bridge(2 * p, l, m, t, before);
    }

    int first_bridge(int p, int l, int r, int t, int before) const {
        if (r < t || before + nodes[p].min_prefix > 0) return -1;
        if (r - l == 1) {
            if (l >= t && before == 0) return l;
            return before + nodes[p].sum == 0 ? r : -1;
        }
        int m = (l + r) / 2;
        int res = first_bridge(2 * p, l, m, t, before);
        if (res != -1) return res;
        return first_bridge(2 * p + 1, m, r, t, before + nodes[2 * p].sum);
    }

    void add_entry(int p, Change& change) {
        if (p < _n) return;
        change.added.push_back(Entry{p - _n, *values[p - _n]});
        _size++;
    }

    void remove_entry(int p, Change& change) {
        if (p < _n) return;
        change.removed.push_back(Entry{p - _n, *values[p - _n]});
        _size--;
    }

    void erase_operation(int t, Change& change) {
        int p = _n + t;
        Node old = nodes[capacity + p];
        if (values[t]) {
            if (old.live != -1) {
                remove_entry(p, change);
                set(p, Node{});
            } else {
                int b = first_bridge(1, 0, capacity, p + 1, 0);
                int candidate = prod(0, b).live;
                assert(candidate != -1);
                remove_entry(candidate, change);
                set(candidate, Node{1, 0, -1, candidate});
                set(p, Node{});
            }
            values[t].reset();
            push_count--;
        } else if (old.sum == -1) {
            int b = last_bridge(1, 0, capacity, p, 0);
            int candidate = prod(b, 2 * _n).dead;
            assert(candidate != -1);
            set(p, Node{});
            set(candidate, Node{0, 0, candidate, -1});
            add_entry(candidate, change);
            pop_count--;
        }
    }

    // Removing and reinserting an operation can temporarily change another
    // push's membership. Suppress that pair, but keep both old/new entries
    // when the push at the edited time itself was replaced.
    static void cancel_unchanged(Change& change, int t) {
        if (change.added.size() == 1 && change.removed.size() == 1 &&
            change.added[0].time == change.removed[0].time &&
            change.added[0].time != t) {
            change.added.clear();
            change.removed.clear();
        }
    }
};

}  // namespace yosupo
