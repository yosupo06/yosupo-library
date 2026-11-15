#pragma once

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"

namespace yosupo {

/**
 * @brief LinkCutTree（動的な森のパス集約・パス更新）
 * @tparam M SplayTree と同じ acted_monoid。monoid は非可換でもよい。
 *
 * 頂点 0, ..., n - 1 に値を持つ森を管理する。頂点数は構築後に変わらない。
 * パスの両端を含め、始点から終点の順で集約する。部分木集約は扱わない。
 * 遅延更新が不要なら ActedMonoid(monoid) を渡す。
 *
 * mapping(f, op(a, b)) = op(mapping(f, a), mapping(f, b))、
 * mapping(act.op(f, g), a) = mapping(f, mapping(g, a))、
 * mapping(act.e, a) = a を満たすこと。パス長が必要な作用では S に長さも
 * 持たせる（例: S = {和, 長さ}）。F の等値比較は不要。
 *
 * @pre n < INT_MAX。全演算の中間値が S, F の表現可能範囲に収まること。
 * @note 全ての頂点引数は [0, n) 内。問い合わせも内部の補助木を変更する。
 * evert, link, cut, prod, apply は下記の仕様に従って森の根も変更する。
 * それ以外の操作は根を変えない。自己ループ・多重辺・閉路は許さない。
 * 使用例は test/oj/linkcuttree_*.test.cpp を参照。
 * @par 計算量
 * S, F のコピー・演算を O(1) とすると、構築は時間・空間 O(n)、
 * size は O(1)、その他の公開操作はならし O(log(n + 1))。
 */
template <acted_monoid M> struct LinkCutTree {
    using S = typename M::S;
    using F = typename M::F;

    /// @brief 値が monoid.e の孤立頂点を n 個作る。n >= 0。
    explicit LinkCutTree(int n, const M& _m = M())
        : LinkCutTree(std::vector<S>(n, _m.monoid.e), _m) {}

    /// @brief 値が v[i] の孤立頂点 i を作る。空の v も許す。
    explicit LinkCutTree(const std::vector<S>& v, const M& _m = M()) : m(_m) {
        nodes.reserve(v.size() + 1);
        nodes.push_back(
            {0, 0, 0, m.monoid.e, m.monoid.e, m.monoid.e, m.act.e, false});
        for (const S& s : v) {
            nodes.push_back({0, 0, 0, s, s, s, m.act.e, false});
        }
    }

    /// @brief 頂点数を返す。
    size_t size() const { return nodes.size() - 1; }

    /// @brief 頂点 v の値を返す。
    S get(int v) {
        check_vertex(v);
        access(++v);
        return nodes[v].s;
    }

    /// @brief 頂点 v の値を s に置き換える。
    void set(int v, S s) {
        check_vertex(v);
        access(++v);
        nodes[v].s = std::move(s);
        update(v);
    }

    /// @brief v を含む木の根を v にする。
    void evert(int v) {
        check_vertex(v);
        access(++v);
        reverse(v);
    }

    /// @brief v を含む木の現在の根を返す。
    int root(int v) {
        check_vertex(v);
        access(++v);
        push(v);
        while (nodes[v].l) {
            v = nodes[v].l;
            push(v);
        }
        splay(v);
        return v - 1;
    }

    /// @brief u と v が同じ木に属するかを返す。u == v なら true。
    bool same(int u, int v) {
        int r = root(u);
        return r == root(v);
    }

    /**
     * @brief 辺 {u, v} を追加する。結合後の根は結合前の v 側の根。
     * @pre u と v は異なる木に属する。
     */
    void link(int u, int v) {
        check_vertex(v);
        evert(u);
        assert(root(v) != u);
        nodes[u + 1].p = v + 1;
    }

    /**
     * @brief 辺 {u, v} を削除する。分割後の根はそれぞれ u, v。
     * @pre 辺 {u, v} が存在する。
     */
    void cut(int u, int v) {
        expose_path(u, v);
        ++u;
        ++v;
        assert(nodes[v].l == u && nodes[u].r == 0);
        nodes[v].l = 0;
        nodes[u].p = 0;
        update(v);
    }

    /**
     * @brief u から v へのパス上の値の積を返す。操作後の根は u。
     * @pre u と v は同じ木に属する。
     * @note 両端を含む。u == v の場合はその頂点の値。
     */
    S prod(int u, int v) {
        expose_path(u, v);
        return nodes[v + 1].prod;
    }

    /**
     * @brief u から v へのパス上の各頂点に f を作用させる。根は u になる。
     * @pre u と v は同じ木に属する。
     * @note 両端を含む。u == v の場合はその頂点のみ更新する。
     */
    void apply(int u, int v, const F& f) {
        expose_path(u, v);
        all_apply(v + 1, f);
    }

    /**
     * @brief 現在の根に対する u, v の LCA を返す。非連結なら -1。
     * @note 根を指定する場合は先に evert を呼ぶ。u == v なら u を返す。
     */
    int lca(int u, int v) {
        if (!same(u, v)) return -1;
        access(u + 1);
        return access(v + 1) - 1;
    }

  private:
    M m;
    struct Node {
        // 補助木の根の p は、元の木での path-parent を表す。
        int l, r, p;
        S s, prod, rprod;
        F f;
        bool rev;
    };
    std::vector<Node> nodes;
    std::vector<int> ancestors;

    void check_vertex(int v) const {
        assert(0 <= v && v < int(size()));
        (void)v;
    }

    bool is_root(int v) const {
        int p = nodes[v].p;
        return p == 0 || (nodes[p].l != v && nodes[p].r != v);
    }

    void update(int v) {
        Node& n = nodes[v];
        n.prod =
            m.monoid.op(m.monoid.op(nodes[n.l].prod, n.s), nodes[n.r].prod);
        n.rprod =
            m.monoid.op(m.monoid.op(nodes[n.r].rprod, n.s), nodes[n.l].rprod);
    }

    void reverse(int v) {
        if (v == 0) return;
        Node& n = nodes[v];
        std::swap(n.l, n.r);
        std::swap(n.prod, n.rprod);
        n.rev = !n.rev;
    }

    void all_apply(int v, const F& f) {
        if (v == 0) return;
        Node& n = nodes[v];
        n.s = m.mapping(f, n.s);
        n.prod = m.mapping(f, n.prod);
        n.rprod = m.mapping(f, n.rprod);
        n.f = m.act.op(f, n.f);
    }

    void push(int v) {
        Node& n = nodes[v];
        if (n.rev) {
            reverse(n.l);
            reverse(n.r);
            n.rev = false;
        }
        all_apply(n.l, n.f);
        all_apply(n.r, n.f);
        n.f = m.act.e;
    }

    void rotate(int v) {
        int p = nodes[v].p, g = nodes[p].p;
        if (!is_root(p)) {
            (nodes[g].l == p ? nodes[g].l : nodes[g].r) = v;
        }
        nodes[v].p = g;
        int c;
        if (nodes[p].l == v) {
            c = nodes[v].r;
            nodes[p].l = c;
            nodes[v].r = p;
        } else {
            c = nodes[v].l;
            nodes[p].r = c;
            nodes[v].l = p;
        }
        if (c) nodes[c].p = p;
        nodes[p].p = v;
        update(p);
        update(v);
    }

    void splay(int v) {
        ancestors.clear();
        for (int u = v;; u = nodes[u].p) {
            ancestors.push_back(u);
            if (is_root(u)) break;
        }
        for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) {
            push(*it);
        }
        while (!is_root(v)) {
            int p = nodes[v].p, g = nodes[p].p;
            if (!is_root(p)) {
                bool zigzig = (nodes[p].l == v) == (nodes[g].l == p);
                rotate(zigzig ? p : v);
            }
            rotate(v);
        }
    }

    int access(int v) {
        int last = 0;
        for (int u = v; u; u = nodes[u].p) {
            splay(u);
            // 外した右の子の p は path-parent として残す。
            nodes[u].r = last;
            if (last) nodes[last].p = u;
            update(u);
            last = u;
        }
        splay(v);
        return last;
    }

    void expose_path(int u, int v) {
        check_vertex(v);
        evert(u);
        assert(root(v) == u);
        access(v + 1);
    }
};

}  // namespace yosupo
