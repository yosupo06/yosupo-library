#include <sys/types.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"

namespace yosupo {

template <acted_monoid M> struct SplayTree {
    using S = typename M::S;
    using F = typename M::F;

    SplayTree(const M& _m) : m(_m) {}

    struct Tree {
        int id, ex;
        Tree() : id(-12345), ex(-12354) {}
        Tree(int _id, int _ex) : id(_id), ex(_ex) {}
        bool empty() const { return id == -12345; }
    };

    Tree build() { return Tree(); }
    Tree build(S s) {
        int id = int(nodes.size());
        nodes.push_back({
            Inner{},
            Leaf{s},
        });
        return Tree(2 * id + 1, 2 * id);
    }
    Tree build(const std::vector<S>& v) {
        nodes.reserve(nodes.size() + v.size());
        Tree t = _build(v, 0, int(v.size()));
        return t;
    }

    size_t size(const Tree& t) {
        if (t.empty()) return 0;
        return size(t.id);
    }
    std::ptrdiff_t ssize(const Tree& t) { return ptrdiff_t(size(t)); }

    Tree merge(Tree&& l, Tree&& r) {
        if (l.empty()) return r;
        if (r.empty()) return l;
        inner(l.ex) = Inner{l.id, r.id, -1, false, m.monoid.e, m.act.e};
        update(l.ex);
        return Tree(l.ex, r.ex);
    }

    Tree split(Tree& t, int k) {
        if (k == 0) return std::exchange(t, build());
        if (k == size(t.id)) return build();
        splay_k(t, k);
        int id = t.id;
        t.id = inner(id).lid;
        return Tree(inner(id).rid, id);
    }

    void insert(Tree& t, int k, S s) {
        assert(0 <= k && k <= ssize(t));
        auto t2 = split(t, k);
        t = merge(std::move(t), build(s));
        t = merge(std::move(t), std::move(t2));
    }

    void erase(Tree& t, int k) {
        assert(0 <= k && k < ssize(t));
        auto t2 = split(t, k);
        auto t3 = split(t2, 1);
        t = merge(std::move(t), std::move(t3));
    }

    S get(Tree& t, int k) {
        assert(0 <= k && k < ssize(t));
        S s;
        access_leaf_k(t, k, [&](int id) { s = leaf(id).s; });
        return s;
    }
    void set(Tree& t, int k, S s) {
        assert(0 <= k && k < ssize(t));
        access_leaf_k(t, k, [&](int id) { leaf(id).s = s; });
    }

    S all_prod(const Tree& t) {
        if (t.empty()) return m.monoid.e;
        return all_prod(t.id);
    }
    S prod(Tree& t, int l, int r) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto t3 = split(t, r);
        auto t2 = split(t, l);
        S s = all_prod(t2);
        t = merge(std::move(t), std::move(t2));
        t = merge(std::move(t), std::move(t3));
        return s;
    }

    void all_apply(Tree& t, F f) {
        if (t.empty()) return;
        all_apply(t.id, f);
    }
    void apply(Tree& t, int l, int r, F f) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto t3 = split(t, r);
        auto t2 = split(t, l);
        all_apply(t2, f);
        t = merge(std::move(t), std::move(t2));
        t = merge(std::move(t), std::move(t3));
    }

    template <class F> int max_right(Tree& t, F f) {
        if (f(all_prod(t))) return int(ssize(t));
        if (ssize(t) == 1) return 0;
        S s = m.monoid.e;
        int r = 0;
        splay(t, [&](int lid, int) {
            S s2 = m.monoid.op(s, all_prod(lid));

            if (!f(s2)) return -1;

            r += size(lid);
            s = s2;
            return 1;
        });
        return r;
    }

    std::vector<S> to_vec(const Tree& t) {
        if (t.empty()) return {};
        std::vector<S> buf;
        buf.reserve(ssize(t.id));
        _to_vec(t.id, buf);
        return buf;
    }

  private:
    M m;

    struct Inner {
        int lid, rid, sz;
        bool lz;
        S s;
        F f;
    };
    struct Leaf {
        S s;
    };
    std::vector<std::pair<Inner, Leaf>> nodes;
    bool leaf_id(int id) { return id % 2; }
    Inner& inner(int id) { return nodes[id / 2].first; }
    Leaf& leaf(int id) { return nodes[id / 2].second; }

    int size(int id) { return leaf_id(id) ? 1 : inner(id).sz; }
    ssize_t ssize(int id) { return leaf_id(id) ? 1 : inner(id).sz; }
    S all_prod(int id) { return leaf_id(id) ? leaf(id).s : inner(id).s; }

    Tree _build(const std::vector<S>& v, int l, int r) {
        if (r - l == 1) {
            return build(v[l]);
        }
        int md = (l + r) / 2;
        return merge(_build(v, l, md), _build(v, md, r));
    }

    void _to_vec(int id, std::vector<S>& buf) {
        if (leaf_id(id)) {
            buf.push_back(leaf(id).s);
            return;
        }
        push(id);
        int lid = inner(id).lid, rid = inner(id).rid;
        _to_vec(lid, buf);
        _to_vec(rid, buf);
    }

    void all_apply(int id, F f) {
        if (leaf_id(id)) {
            Leaf& n = leaf(id);
            n.s = m.mapping(f, n.s);
        } else {
            Inner& n = inner(id);
            n.s = m.mapping(f, n.s);
            n.f = m.act.op(f, n.f);
            n.lz = true;
        }
    }

    void push(int id) {
        Inner& n = inner(id);

        if (n.lz) {
            all_apply(n.lid, n.f);
            all_apply(n.rid, n.f);
            n.f = m.act.e;
            n.lz = false;
        }
    }

    void update(int id) {
        Inner& n = inner(id);
        n.sz = size(n.lid) + size(n.rid);
        n.s = m.monoid.op(all_prod(n.lid), all_prod(n.rid));
    }

    template <class F> void splay(Tree& t, F f) {
        assert(!leaf_id(t.id));
        static std::vector<int> lefts, rights;
        lefts.clear();
        rights.clear();
        int zig = 0;
        int id = t.id;
        while (true) {
            push(id);
            int lid = inner(id).lid, rid = inner(id).rid;

            auto dir = f(lid, rid);

            if (dir == -1 && size(lid) > 1) {
                if (zig == -1) {
                    inner(rights.back()).lid = rid;
                    update(rights.back());
                    inner(id).rid = rights.back();
                    rights.pop_back();
                }
                rights.push_back(id);

                id = lid;
                if (zig == 0) {
                    zig = -1;
                } else {
                    zig = 0;
                }
            } else if (dir == 1 && size(rid) > 1) {
                if (zig == 1) {
                    inner(lefts.back()).rid = lid;
                    update(lefts.back());
                    inner(id).lid = lefts.back();
                    lefts.pop_back();
                }
                lefts.push_back(id);

                id = rid;
                if (zig == 0) {
                    zig = 1;
                } else {
                    zig = 0;
                }
            } else {
                {
                    int tmp = lid;
                    for (auto i = std::ssize(lefts) - 1; i >= 0; i--) {
                        inner(lefts[i]).rid = std::exchange(tmp, lefts[i]);
                        update(lefts[i]);
                    }
                    inner(id).lid = tmp;
                }
                {
                    int tmp = rid;
                    for (auto i = std::ssize(rights) - 1; i >= 0; i--) {
                        inner(rights[i]).lid = std::exchange(tmp, rights[i]);
                        update(rights[i]);
                    }
                    inner(id).rid = tmp;
                }
                update(id);
                break;
            }
        }
        t.id = id;
    }

    void splay_k(Tree& t, int k) {
        splay(t, [&](int lid, int) {
            int lsz = size(lid);
            if (k == lsz) return 0;
            if (k < lsz) return -1;
            k -= lsz;
            return 1;
        });
    }

    template <class F, class G> void access_leaf(Tree& t, F f, G g) {
        assert(ssize(t));
        if (ssize(t) == 1) {
            g(t.id);
            return;
        }
        splay(t, [&](int lid, int rid) {
            int dir = f(lid, rid);
            if (dir == -1 && ssize(lid) == 1) {
                g(lid);
            }
            if (dir == 1 && ssize(rid) == 1) {
                g(rid);
            }
            return dir;
        });
    }
    template <class F> void access_leaf_k(Tree& t, int k, F f) {
        assert(0 <= k && k < ssize(t));
        access_leaf(
            t,
            [&](int lid, int) {
                int lsz = size(lid);
                if (k < lsz) return -1;
                k -= lsz;
                return 1;
            },
            f);
    }
};

}  // namespace yosupo
