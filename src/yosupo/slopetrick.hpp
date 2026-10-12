#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <utility>

#include "yosupo/random.hpp"

namespace yosupo {

// A convex piecewise-linear function on a nonempty closed interval (possibly
// unbounded). Coordinates and slopes use T; function values use V.
// See docs/slopetrick.md for semantics, examples, and complexity.
template <class T = long long, class V = T> struct SlopeTrick {
    static_assert(std::numeric_limits<T>::is_integer &&
                  std::numeric_limits<T>::is_signed);
    static_assert(std::numeric_limits<V>::is_integer &&
                  std::numeric_limits<V>::is_signed);

    struct Interval {
        std::optional<T> lower, upper;  // nullopt means -infinity / +infinity
        bool operator==(const Interval&) const = default;
    };

    // f(x) = c on the whole line, or on [l, r].
    explicit SlopeTrick(V c = 0) : intercept(c) {}
    SlopeTrick(T l, T r, V c = 0) : bounds{l, r}, intercept(c) {
        assert(l <= r);
    }
    SlopeTrick(const SlopeTrick& other)
        : root(clone(other.root)),
          bounds(other.bounds),
          slope(other.slope),
          intercept(other.intercept) {}
    SlopeTrick(SlopeTrick&& other) noexcept { swap(other); }
    SlopeTrick& operator=(SlopeTrick other) {
        swap(other);
        return *this;
    }
    void swap(SlopeTrick& other) noexcept {
        root.swap(other.root);
        std::swap(bounds, other.bounds);
        std::swap(slope, other.slope);
        std::swap(intercept, other.intercept);
    }

    Interval domain() const { return bounds; }
    int size() const { return count(root); }  // number of interior breakpoints

    // nullopt means x is outside the domain (+infinity).
    std::optional<V> eval(T x) const {
        if ((bounds.lower && x < *bounds.lower) ||
            (bounds.upper && *bounds.upper < x)) {
            return std::nullopt;
        }
        V value = V(slope) * V(x) + intercept;
        const Node* p = root.get();
        T offset = 0;
        while (p) {
            T key = p->x + offset;
            if (key <= x) {
                T child_offset = offset + p->lazy;
                value += (V(x) - V(child_offset)) * V(weight(p->left)) -
                         moment(p->left) + V(p->w) * (V(x) - V(key));
                offset = child_offset;
                p = p->right.get();
            } else {
                offset += p->lazy;
                p = p->left.get();
            }
        }
        return value;
    }

    bool bounded_below() const {
        return (bounds.lower || slope <= 0) &&
               (bounds.upper || slope + weight(root) >= 0);
    }
    // Precondition: bounded_below(). Endpoints are inclusive.
    Interval argmin() const {
        assert(bounded_below());
        T total = weight(root);
        Interval result;
        if (slope >= 0) {
            result.lower = bounds.lower;
        } else if (slope + total < 0) {
            result.lower = bounds.upper;
        } else {
            result.lower = crossing(-slope, false);
        }
        if (slope > 0) {
            result.upper = bounds.lower;
        } else if (slope + total <= 0) {
            result.upper = bounds.upper;
        } else {
            result.upper = crossing(-slope, true);
        }
        return result;
    }
    V min() const {
        auto range = argmin();
        return *eval(range.lower.value_or(range.upper.value_or(T(0))));
    }

    void add_const(V c) { intercept += c; }
    // f(x) += a*x + b. The result need not be bounded below.
    void add_linear(T a, V b = 0) {
        slope += a;
        intercept += b;
    }
    // f(x) += w*max(x-a, 0), w*max(a-x, 0), or w*abs(x-a).
    void add_x_minus_a(T a, T w = 1) {
        assert(w >= 0);
        if (w == 0) return;
        if (bounds.lower && a <= *bounds.lower) {
            add_linear(w, -V(w) * V(a));
        } else if (!bounds.upper || a < *bounds.upper) {
            insert(root, a, w);
        }
    }
    void add_a_minus_x(T a, T w = 1) {
        assert(w >= 0);
        add_linear(-w, V(w) * V(a));
        add_x_minus_a(a, w);
    }
    void add_abs(T a, T w = 1) {
        add_a_minus_x(a, w);
        add_x_minus_a(a, w);
    }

    // Intersect the domain with [l, r]. The intersection must be nonempty.
    void restrict_domain(std::optional<T> l, std::optional<T> r) {
        if (l && (!bounds.lower || *bounds.lower < *l)) bounds.lower = l;
        if (r && (!bounds.upper || *r < *bounds.upper)) bounds.upper = r;
        assert(!bounds.lower || !bounds.upper ||
               *bounds.lower <= *bounds.upper);
        if (bounds.lower) {
            auto [left, right] = split(std::move(root), *bounds.lower, true);
            slope += weight(left);
            intercept -= moment(left);
            root = std::move(right);
        }
        if (bounds.upper) {
            auto [left, right] = split(std::move(root), *bounds.upper, false);
            root = std::move(left);
        }
    }

    // f_new(x) = f_old(x-a).
    void shift(T a) {
        translate(root, a);
        intercept -= V(slope) * V(a);
        if (bounds.lower) *bounds.lower += a;
        if (bounds.upper) *bounds.upper += a;
    }

    // f_new(x) = min_{x-b <= y <= x-a} f_old(y), a <= b.
    void shift_min(T a, T b) {
        assert(a <= b);
        if (a == b) {
            shift(a);
            return;
        }
        if (!bounded_below()) {
            // A convex function unbounded below is monotone.
            shift(slope > 0 ? b : a);
            return;
        }
        auto range = argmin();
        T p = range.lower.value_or(range.upper.value_or(T(0)));
        V value = *eval(p);
        // Split the slope jump at a minimizer into negative -> 0 -> positive.
        // Move the decreasing part by a, and the increasing part by b.
        auto [left, rest] = split(std::move(root), p, false);
        auto [middle, right] = split(std::move(rest), p, true);
        T left_slope = slope + weight(left);
        T right_slope = left_slope + weight(middle);
        if (bounds.lower && p == *bounds.lower) slope = left_slope = 0;
        if (bounds.upper && p == *bounds.upper) right_slope = 0;
        translate(left, a);
        translate(right, b);
        T q = p + a;
        intercept =
            value - V(slope) * V(q) - V(weight(left)) * V(q) + moment(left);
        if (left_slope < 0) insert(left, q, -left_slope);
        if (right_slope > 0) insert(right, p + b, right_slope);
        root = join(std::move(left), std::move(right));
        if (bounds.lower) *bounds.lower += a;
        if (bounds.upper) *bounds.upper += b;
    }

    // f_new(x) = min_{y <= x} f_old(y). Must not produce -infinity.
    void prefix_min() {
        assert(bounds.lower || slope <= 0);
        if (!bounded_below()) return;  // decreasing to -infinity on the right
        auto p = argmin().lower;
        V value = min();
        if (!p || p == bounds.lower) {
            root.reset();
            slope = 0;
            intercept = value;
            bounds.upper.reset();
            return;
        }
        restrict_domain(std::nullopt, p);
        bounds.upper.reset();
        add_x_minus_a(*p, -(slope + weight(root)));
    }
    // f_new(x) = min_{y >= x} f_old(y). Must not produce -infinity.
    void suffix_min() {
        assert(bounds.upper || slope + weight(root) >= 0);
        if (!bounded_below()) return;  // increasing from -infinity on the left
        auto p = argmin().upper;
        V value = min();
        if (!p || p == bounds.upper) {
            root.reset();
            slope = 0;
            intercept = value;
            bounds.lower.reset();
            return;
        }
        restrict_domain(p, std::nullopt);
        bounds.lower.reset();
        add_a_minus_x(*p, slope);
    }

    // f += other, intersecting domains. Pass std::move(other) to consume it;
    // an lvalue is copied. The smaller breakpoint tree is inserted into the
    // larger one, so repeated tree-DP merges can use small-to-large merging.
    void add(SlopeTrick other) {
        if (size() < other.size()) swap(other);
        restrict_domain(other.bounds.lower, other.bounds.upper);
        add_linear(other.slope, other.intercept);
        auto visit = [&](auto&& self, Ptr& p) -> void {
            if (!p) return;
            push(p);
            self(self, p->left);
            add_x_minus_a(p->x, p->w);
            self(self, p->right);
        };
        visit(visit, other.root);
    }

  private:
    struct Node;
    using Ptr = std::unique_ptr<Node>;
    struct Node {
        T x, w, sum_w, lazy = 0;
        V sum_xw;
        int count = 1;
        uint64_t priority;
        Ptr left, right;
        Node(T x_, T w_)
            : x(x_),
              w(w_),
              sum_w(w_),
              sum_xw(V(x_) * V(w_)),
              priority(global_gen()()) {}
    };
    Ptr root;
    Interval bounds{};
    // On the domain: f(x) = slope*x + intercept + sum w*max(x-key, 0).
    // Keys are distinct and strictly inside the domain, and weights are > 0.
    T slope = 0;
    V intercept = 0;

    static T weight(const Ptr& p) { return p ? p->sum_w : T(0); }
    static V moment(const Ptr& p) { return p ? p->sum_xw : V(0); }
    static int count(const Ptr& p) { return p ? p->count : 0; }
    static void pull(Ptr& p) {
        p->sum_w = weight(p->left) + p->w + weight(p->right);
        p->sum_xw = moment(p->left) + V(p->x) * V(p->w) + moment(p->right);
        p->count = count(p->left) + 1 + count(p->right);
    }
    static void translate(Ptr& p, T a) {
        if (!p) return;
        p->x += a;
        p->lazy += a;
        p->sum_xw += V(p->sum_w) * V(a);
    }
    static void push(Ptr& p) {
        translate(p->left, p->lazy);
        translate(p->right, p->lazy);
        p->lazy = 0;
    }
    static Ptr clone(const Ptr& p) {
        if (!p) return nullptr;
        auto q = std::make_unique<Node>(p->x, p->w);
        q->sum_w = p->sum_w;
        q->sum_xw = p->sum_xw;
        q->lazy = p->lazy;
        q->count = p->count;
        q->priority = p->priority;
        q->left = clone(p->left);
        q->right = clone(p->right);
        return q;
    }
    // All keys in left must be strictly smaller than all keys in right.
    static Ptr join(Ptr left, Ptr right) {
        if (!left) return right;
        if (!right) return left;
        if (left->priority < right->priority) {
            push(left);
            left->right = join(std::move(left->right), std::move(right));
            pull(left);
            return left;
        }
        push(right);
        right->left = join(std::move(left), std::move(right->left));
        pull(right);
        return right;
    }
    // Split into (< x, >= x), or (<= x, > x) when inclusive is true.
    static std::pair<Ptr, Ptr> split(Ptr p, T x, bool inclusive) {
        if (!p) return {};
        push(p);
        if (p->x < x || (inclusive && p->x == x)) {
            auto [left, right] = split(std::move(p->right), x, inclusive);
            p->right = std::move(left);
            pull(p);
            return {std::move(p), std::move(right)};
        }
        auto [left, right] = split(std::move(p->left), x, inclusive);
        p->left = std::move(right);
        pull(p);
        return {std::move(left), std::move(p)};
    }
    static void insert(Ptr& p, T x, T w) {
        auto [left, rest] = split(std::move(p), x, false);
        auto [middle, right] = split(std::move(rest), x, true);
        if (middle) {
            middle->w += w;
            pull(middle);
        } else {
            middle = std::make_unique<Node>(x, w);
        }
        p = join(join(std::move(left), std::move(middle)), std::move(right));
    }
    // First breakpoint whose cumulative weight is >= target (or > target).
    T crossing(T target, bool strict) const {
        const Node* p = root.get();
        T offset = 0;
        while (p) {
            T w = weight(p->left);
            if (strict ? w > target : w >= target) {
                offset += p->lazy;
                p = p->left.get();
            } else if (strict ? w + p->w > target : w + p->w >= target) {
                return p->x + offset;
            } else {
                target -= w + p->w;
                offset += p->lazy;
                p = p->right.get();
            }
        }
        assert(false);
        return 0;
    }
};

}  // namespace yosupo
