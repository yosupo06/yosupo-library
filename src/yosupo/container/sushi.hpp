#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <numeric>
#include <span>
#include <utility>
#include <vector>

namespace yosupo {

/// @brief 回転寿司クエリのバッチ処理（線形ワード空間）。
/// @note クエリは 0-indexed の半開区間 [l, r) と投入値 x。
template <class T> struct SushiQuery {
    int l, r;
    T x;
};

namespace internal {

// Packed static rank: one prefix count per 64-bit word, not per element.
struct SushiWavelet {
    struct Level {
        std::vector<std::uint64_t> bits;
        std::vector<int> prefix;
        int zeros;
        int rank(int p) const {
            int word = p >> 6, offset = p & 63;
            return prefix[word] +
                   (offset ? std::popcount(bits[word] &
                                           ((std::uint64_t(1) << offset) - 1))
                           : 0);
        }
    };
    std::vector<Level> levels;
    SushiWavelet(std::vector<int> a, int height) {
        int n = int(a.size());
        std::vector<int> next(n);
        for (int b = height - 1; b >= 0; --b) {
            Level level;
            level.bits.assign((n + 63) / 64, 0);
            for (int i = 0; i < n; ++i)
                level.bits[i >> 6] |= std::uint64_t((a[i] >> b) & 1)
                                      << (i & 63);
            level.prefix.resize(level.bits.size() + 1);
            for (int i = 0; i < int(level.bits.size()); ++i)
                level.prefix[i + 1] =
                    level.prefix[i] + std::popcount(level.bits[i]);
            level.zeros = n - level.prefix.back();
            int left = 0, right = level.zeros;
            for (int x : a) next[((x >> b) & 1) ? right++ : left++] = x;
            a.swap(next);
            levels.push_back(std::move(level));
        }
    }
};

// Empty is structural: signed sums may cancel in a nonempty subtree.
struct SushiDeltaTrie {
    struct Node {
        int child[2] = {};
        int key = 0, sum = 0, bit = -1;
    };
    std::vector<Node> nodes{Node{}};
    int free_head = 0;
    int allocate(Node node) {
        if (free_head) {
            int p = free_head;
            free_head = nodes[p].child[0];
            nodes[p] = node;
            return p;
        }
        nodes.push_back(node);
        return int(nodes.size()) - 1;
    }
    void release(int p) {
        nodes[p].child[0] = free_head;
        free_head = p;
    }
    int sum(int p) const { return nodes[p].sum; }
    int add(int p, int key, int delta) {
        if (!p) return allocate({{}, key, delta, -1});
        Node node = nodes[p];
        int bit = int(std::bit_width(unsigned(key ^ node.key))) - 1;
        if (bit > node.bit) {
            int leaf = allocate({{}, key, delta, -1});
            int side = (key >> bit) & 1;
            Node branch;
            branch.child[side] = leaf;
            branch.child[side ^ 1] = p;
            branch.key = nodes[branch.child[0]].key;
            branch.sum = node.sum + delta;
            branch.bit = bit;
            return allocate(branch);
        }
        if (node.bit == -1) {
            assert(node.key == key);
            nodes[p].sum += delta;
            if (nodes[p].sum) return p;
            release(p);
            return 0;
        }
        int side = (key >> node.bit) & 1;
        node.child[side] = add(node.child[side], key, delta);
        if (!node.child[side]) {
            release(p);
            return node.child[side ^ 1];
        }
        node.key = nodes[node.child[0]].key;
        node.sum += delta;
        nodes[p] = node;
        return p;
    }
    std::array<int, 2> split(int p, int bit) const {
        if (!p) return {0, 0};
        const Node& node = nodes[p];
        if (node.bit == bit) return {node.child[0], node.child[1]};
        assert(node.bit < bit);
        return ((node.key >> bit) & 1) ? std::array{0, p} : std::array{p, 0};
    }
    // Only for sets with empty baseline (N and H): all weights are positive.
    int minimum(int p) const {
        assert(p && sum(p) > 0);
        return nodes[p].key;
    }
};

template <class T> class SushiEpoch {
    enum { N, D, L, H };
    struct TimeNode {
        int roots[4] = {};
        // L's maximum; at a leaf, the sole D key. -1 denotes empty.
        int boundary = -1;
    };
    struct Change {
        int key, delta;
    };
    struct Changes {
        std::array<Change, 8> items;
        int count = 0;
        void add(int key, int delta) {
            for (int i = 0; i < count; ++i) {
                if (items[i].key != key) continue;
                items[i].delta += delta;
                if (!items[i].delta) items[i] = items[--count];
                return;
            }
            assert(count < int(items.size()));
            items[count++] = {key, delta};
        }
    };
    struct Delta {
        Changes n, d;
    };
    int n, size, height;
    int largest_live_key = -1;
    std::vector<TimeNode> tree;
    SushiDeltaTrie trie;
    std::vector<T> values;
    std::vector<int> position, input_keys;
    SushiWavelet wm;

    // Return ranks in descending value order; never negate T (signed minimum).
    static std::vector<int> rank_keys(const std::vector<T>& a,
                                      std::span<const SushiQuery<T>> qs) {
        int count = int(a.size() + qs.size());
        std::vector<int> order(count), ranks(count);
        std::iota(order.begin(), order.end(), 0);
        auto value = [&](int i) -> const T& {
            return i < int(a.size()) ? a[i] : qs[i - int(a.size())].x;
        };
        std::sort(order.begin(), order.end(), [&](int i, int j) {
            if (value(j) < value(i)) return true;
            if (value(i) < value(j)) return false;
            return i < j;
        });
        for (int i = 0; i < count; ++i) ranks[order[i]] = i;
        return ranks;
    }
    SushiEpoch(const std::vector<T>& a,
               std::span<const SushiQuery<T>> qs,
               std::vector<int> ranks)
        : n(int(a.size())),
          size(int(std::bit_ceil(unsigned(n)))),
          height(std::max(1, int(std::bit_width(unsigned(ranks.size() - 1))))),
          tree(2 * size),
          values(ranks.size()),
          position(ranks.size(), -1),
          input_keys(ranks.begin() + n, ranks.end()),
          wm(std::vector<int>(ranks.begin(), ranks.begin() + n), height) {
        trie.nodes.reserve(std::max(n, 1));
        std::vector<int> maxima(2 * size, -1);
        for (int i = 0; i < n; ++i) {
            values[ranks[i]] = a[i];
            position[ranks[i]] = i;
            maxima[size + i] = tree[size + i].boundary = ranks[i];
        }
        for (int i = 0; i < int(qs.size()); ++i)
            values[input_keys[i]] = qs[i].x;
        for (int v = size - 1; v; --v) {
            tree[v].boundary = maxima[2 * v + 1];
            maxima[v] = std::max(maxima[2 * v], maxima[2 * v + 1]);
        }
        largest_live_key = maxima[1];
    }
    void change(int v, int which, int key, int delta, Delta& out) {
        tree[v].roots[which] = trie.add(tree[v].roots[which], key, delta);
        if (which == N) out.n.add(key, delta);
        if (which == D) out.d.add(key, delta);
    }
    int maximum_l(int v, int mid, int end) const {
        int l = std::min(mid, n), r = std::min(end, n), t = tree[v].roots[L];
        int count = r - l + trie.sum(t);
        if (!count) return -1;
        int key = 0;
        for (int level = 0, bit = height - 1; bit >= 0; ++level, --bit) {
            const auto& w = wm.levels[level];
            int lo = w.rank(l), hi = w.rank(r);
            auto children = trie.split(t, bit);
            int right_count = hi - lo + trie.sum(children[1]);
            int side = int(right_count != 0);
            assert(right_count >= 0);
            if (side) {
                l = w.zeros + lo;
                r = w.zeros + hi;
            } else {
                l -= lo;
                r -= hi;
            }
            key = (key << 1) | side;
            t = children[side];
        }
        return key;
    }
    void partition_changes(int v,
                           int mid,
                           int end,
                           const Changes& edits,
                           Delta& out) {
        if (!edits.count) return;
        int capacity = std::min(end, n) - std::min(mid, n);
        int boundary = tree[v].boundary;
        bool invalid_boundary = false;
        auto low = [&](int x, int d) {
            change(v, L, x, d, out);
            change(v, D, x, d, out);
        };
        auto high = [&](int x, int d) {
            change(v, H, x, d, out);
            change(v, N, x, d, out);
        };
        // Classify against the old boundary, then rebalance once. A replacement
        // in D must not trigger two independent WM boundary searches.
        for (int i = 0; i < edits.count; ++i) {
            auto [key, delta] = edits.items[i];
            assert(delta == -1 || delta == 1);
            if (key <= boundary) {
                low(key, delta);
                if (key == boundary && delta == -1) invalid_boundary = true;
            } else
                high(key, delta);
        }
        int low_size = capacity + trie.sum(tree[v].roots[L]);
        while (low_size > capacity) {
            if (invalid_boundary) boundary = maximum_l(v, mid, end);
            low(boundary, -1);
            high(boundary, 1);
            invalid_boundary = true;
            --low_size;
        }
        while (low_size < capacity && tree[v].roots[H]) {
            boundary = trie.minimum(tree[v].roots[H]);
            high(boundary, -1);
            low(boundary, 1);
            invalid_boundary = false;
            ++low_size;
        }
        tree[v].boundary = invalid_boundary ? maximum_l(v, mid, end) : boundary;
    }
    void edit_history(int key, int p, int delta) {
        position[key] = delta == 1 ? p : -1;
        int v = size + p;
        Delta out;
        int popped = tree[v].boundary;
        if (delta == 1) {
            if (popped == -1 || key < popped) {
                if (popped != -1) {
                    change(v, D, popped, -1, out);
                    change(v, N, popped, 1, out);
                }
                change(v, D, key, 1, out);
                tree[v].boundary = key;
            } else
                change(v, N, key, 1, out);
        } else if (key == popped) {
            change(v, D, key, -1, out);
            int remaining = tree[v].roots[N];
            tree[v].boundary = remaining ? trie.minimum(remaining) : -1;
            if (remaining) {
                int next = tree[v].boundary;
                change(v, N, next, -1, out);
                change(v, D, next, 1, out);
            }
        } else
            change(v, N, key, -1, out);
        for (int length = 2; v > 1; length *= 2) {
            bool right = (v & 1) != 0;
            v >>= 1;
            int start = p & ~(length - 1), mid = start + length / 2,
                end = start + length;
            Delta next;
            const Changes& direct = right ? out.n : out.d;
            for (int i = 0; i < direct.count; ++i)
                change(v, right ? N : D, direct.items[i].key,
                       direct.items[i].delta, next);
            const Changes& merged = right ? out.d : out.n;
            partition_changes(v, mid, end, merged, next);
            // One history edit leaves at most one extra/missing queue item;
            // changed popped keys telescope to at most one replacement.
            assert(next.n.count <= 1 && next.d.count <= 2);
            out = next;
        }
    }
    struct Cursor {
        int l, r, nt, dt, nb, db, length;
    };
    int range_minimum(int l, int r) const {
        // Two canonical covers together contain at most 3*log2(size) nodes.
        std::array<Cursor, 96> parts;
        int count = 0;
        auto append = [&](int v) {
            int depth = int(std::bit_width(unsigned(v))) - 1;
            int length = size >> depth;
            int begin = (v - (1 << depth)) * length;
            parts[count++] = {
                begin, begin + length, tree[v].roots[N], tree[v].roots[D], 0,
                0,     length};
        };
        auto cover = [&](int a, int b) {
            std::array<int, 32> right;
            int nr = 0;
            for (a += size, b += size; a < b; a >>= 1, b >>= 1) {
                if (a & 1) append(a++);
                if (b & 1) right[nr++] = --b;
            }
            while (nr) append(right[--nr]);
        };
        cover(0, l);
        int prefix = count;
        cover(l, r);
        struct Split {
            int lo, hi;
            std::array<int, 2> ns, ds;
            int nc, dc;
        };
        std::array<Split, 96> splits;
        int key = 0;
        for (int level = 0, bit = height - 1; bit >= 0; ++level, --bit) {
            const auto& w = wm.levels[level];
            int carry = 0;
            bool ok = true;
            for (int i = 0; i < count; ++i) {
                const auto& c = parts[i];
                auto& s = splits[i];
                s.lo = w.rank(c.l);
                s.hi = w.rank(c.r);
                s.ns = trie.split(c.nt, bit);
                s.ds = trie.split(c.dt, bit);
                s.nc = trie.sum(s.ns[0]);
                s.dc = (c.r - c.l) - (s.hi - s.lo) + trie.sum(s.ds[0]);
                int small_n = c.nb + s.nc, small_d = c.db + s.dc;
                if (i >= prefix && carry + small_d < c.length) ok = false;
                carry = small_n + std::max(0, carry + small_d - c.length);
            }
            int side = int(!ok);
            key = (key << 1) | side;
            for (int i = 0; i < count; ++i) {
                auto& c = parts[i];
                const auto& s = splits[i];
                if (side) {
                    c.l = w.zeros + s.lo;
                    c.r = w.zeros + s.hi;
                    c.nb += s.nc;
                    c.db += s.dc;
                } else {
                    c.l -= s.lo;
                    c.r -= s.hi;
                }
                c.nt = s.ns[side];
                c.dt = s.ds[side];
            }
        }
        return key;
    }

  public:
    SushiEpoch(const std::vector<T>& a, std::span<const SushiQuery<T>> qs)
        : SushiEpoch(a, qs, rank_keys(a, qs)) {}
    T query(int index, const SushiQuery<T>& q) {
        if (q.l == q.r || !(values[largest_live_key] < q.x)) return q.x;
        int key =
            q.l == 0 && q.r == n ? largest_live_key : range_minimum(q.l, q.r);
        if (!(values[key] < q.x)) return q.x;
        T answer = values[key];
        edit_history(key, position[key], -1);
        edit_history(input_keys[index], q.l, 1);
        // A successful query inserts a strictly larger value (smaller key).
        while (position[largest_live_key] == -1) --largest_live_key;
        return answer;
    }
    void materialize(std::vector<T>& a) const {
        // Schedule keys by priority at their first free time >= insertion time.
        // This gives the same output as the priority queue. Ranks already sort
        // all records, so only a successor DSU is needed, with one
        // word/position.
        std::vector<int> successor(n + 1);
        std::iota(successor.begin(), successor.end(), 0);
        auto find = [&](int p) {
            while (successor[p] != p) {
                successor[p] = successor[successor[p]];
                p = successor[p];
            }
            return p;
        };
        for (int key = 0; key < int(position.size()); ++key) {
            int p = position[key];
            if (p == -1) continue;
            int output = find(p);
            assert(output < n);
            a[output] = values[key];
            successor[output] = find(output + 1);
        }
    }
};
}  // namespace internal

/// @brief
/// 配列を更新する回転寿司クエリのバッチ処理。基準WMと差分Trieを再構築する。
/// @tparam T デフォルト構築・コピー可能で < が全順序をなす値型（bool を除く）。
/// @note 各クエリは [l,r) の順に if (a[i] < x) swap(a[i],x) を行い x を返す。
///       apply
///       内のクエリは先に与える。回答に依存するオンライン処理には向かない。
/// @code
/// SushiBatch<int> sushi({3, 1, 4, 2});
/// auto answers = sushi.apply({{0, 4, 5}}); // {1}, 配列は {5,3,4,2}
/// sushi.set(0, -3);                       // バッチ間の代入
/// @endcode
template <class T> class SushiBatch {
    std::vector<T> a;

  public:
    explicit SushiBatch(std::vector<T> values) : a(std::move(values)) {}
    int size() const { return int(a.size()); }
    /// @brief 現在の配列への参照。オブジェクトの破棄まで有効、更新に追従する。
    const std::vector<T>& values() const { return a; }
    T get(int p) const {
        assert(0 <= p && p < size());
        return a[p];
    }
    /// @brief バッチの間の1点代入。O(1)。
    void set(int p, T x) {
        assert(0 <= p && p < size());
        a[p] = std::move(x);
    }
    /// @brief 小さいクエリ列を波括弧で渡すための apply オーバーロード。
    std::vector<T> apply(std::initializer_list<SushiQuery<T>> queries,
                         int batch_size = 0) {
        return apply(
            std::span<const SushiQuery<T>>(queries.begin(), queries.size()),
            batch_size);
    }
    /// @brief 指定順に処理して回答を返す。配列の変更は次の apply に引き継ぐ。
    /// @param queries 0 <= l <= r <= size() を満たすクエリ列。
    /// @param batch_size 再構築間隔。0 は約 N/(2 log2 N)、正なら指定値。
    /// @pre N と再構築間隔の和、および内部ノード数が int に収まること。
    /// @par 計算量
    /// 自動設定では O(N log N + Q log² N) 時間、回答と入力を除く O(N) words。
    /// 指定間隔 B では O((1+Q/B)N log(N+B)+Q log N log(N+B)) 時間、
    /// O(N log(N+B)/word_bits + N+B log N) words（値型が O(1) words の場合）。
    /// N <= 1 は O(Q)、空クエリ列は O(1)。
    std::vector<T> apply(std::span<const SushiQuery<T>> queries,
                         int batch_size = 0) {
        assert(batch_size >= 0);
        std::vector<T> answers;
        if (queries.empty()) return answers;
        answers.reserve(queries.size());
        for ([[maybe_unused]] const auto& q : queries)
            assert(0 <= q.l && q.l <= q.r && q.r <= size());
        if (size() <= 1) {
            for (const auto& q : queries) {
                T x = q.x;
                if (q.l < q.r && a[0] < x) std::swap(a[0], x);
                answers.push_back(x);
            }
            return answers;
        }
        if (!batch_size)
            batch_size = std::max(
                1, size() / (2 * int(std::bit_width(unsigned(size())))));
        // Every position only increases during apply, so this remains a lower
        // bound even after preceding batches. Skip batches with no swaps.
        const T lower_bound = *std::min_element(a.begin(), a.end());
        for (std::size_t begin = 0; begin < queries.size();) {
            auto batch = queries.subspan(
                begin,
                std::min(std::size_t(batch_size), queries.size() - begin));
            if (std::all_of(batch.begin(), batch.end(), [&](const auto& q) {
                    return q.l == q.r || !(lower_bound < q.x);
                })) {
                for (const auto& q : batch) answers.push_back(q.x);
                begin += batch.size();
                continue;
            }
            internal::SushiEpoch<T> epoch(a, batch);
            for (int i = 0; i < int(batch.size()); ++i)
                answers.push_back(epoch.query(i, batch[i]));
            epoch.materialize(a);
            begin += batch.size();
        }
        return answers;
    }
};

}  // namespace yosupo
