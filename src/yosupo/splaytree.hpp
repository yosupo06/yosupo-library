#include <utility>
#include <vector>
#include <algorithm>

namespace yosupo {

template <class M> struct SplayTree {
    using S = M::S;
    using F = M::F;

    M m;

    struct Inner {
        int lid, rid, sz;
        bool lz, rev;
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

    SplayTree(M _m) : m(_m) {}

    struct Tree {
        int id = -1, ex = -1;
    };

    Tree make_empty() { return Tree{}; }
    Tree make_leaf(S s) {
        int id = int(nodes.size());
        nodes.push_back({
            Inner{},
            Leaf{s},
        });

        return Tree{2 * id + 1, 2 * id};
    }

    S all_prod(int id) { return leaf_id(id) ? leaf(id).s : inner(id).s; }
    S all_prod(const Tree& t) {
        if (t.id == -1) return m.e();
        return all_prod(t.id);
    }

    void all_apply(int id, F f) {
        if (leaf_id(id)) {
            Leaf& n = leaf(id);
            n.s = m.mapping(f, n.s);
        } else {
            Inner& n = inner(id);
            n.s = m.mapping(f, n.s);
            n.f = m.composition(f, n.f);
            n.lz = true;
        }
    }
    void all_apply(Tree& t, F f) {
        if (t.id != -1) all_apply(t.id, f);
    }

    void reverse(int id) {
        if (!leaf_id(id)) {
            Inner& n = inner(id);
            n.rev = !n.rev;
            std::swap(n.lid, n.rid);
            n.s = m.rev(n.s);
        }
    }
    void reverse(Tree& t) {
        if (t.id == -1) return;
        reverse(t.id);
    }

    void push(int id) {
        Inner& n = inner(id);

        if (n.lz) {
            all_apply(n.lid, n.f);
            all_apply(n.rid, n.f);
        }
        if (n.rev) {
            reverse(n.lid);
            reverse(n.rid);
        }

        n.f = m.id();
        n.rev = false;
    }

    void update(int id) {
        Inner& n = inner(id);
        n.sz = size(n.lid) + size(n.rid);
        n.s = m.op(all_prod(n.lid), all_prod(n.rid));
    }

    template <class F> int splay(int id, F f) {
        static std::vector<int> lefts, rights;
        lefts.clear();
        rights.clear();
        int zig = 0;
        while (true) {
            push(id);
            int lid = inner(id).lid, rid = inner(id).rid;

            auto dir = f(lid, rid);

            if (dir == -1) {
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
            } else if (dir == 1) {
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

        return id;
    }

    int splay_k(int id, int k) {
        return splay(id, [&](int lid, int rid) {
            int lsz = size(lid);
            if (k == lsz) return 0;
            if (k < lsz) return -1;
            k -= lsz;
            return 1;
        });
    }

    Tree _build(const std::vector<S>& v, int l, int r) {
        if (r - l == 1) {
            return make_leaf(v[l]);
        }
        int md = (l + r) / 2;
        return merge(_build(v, l, md), _build(v, md, r));
    }
    Tree build(const std::vector<S>& v) {
        nodes.reserve(nodes.size() + v.size());
        Tree t = _build(v, 0, int(v.size()));
        return t;
    }

    Tree merge(Tree l, Tree r) {
        if (l.id == -1) return r;
        if (r.id == -1) return l;
        inner(l.ex) = Inner{l.id, r.id, -1, false, false, m.e(), m.id()};
        update(l.ex);
        return Tree{l.ex, r.ex};
    }

    Tree split(Tree& t, int k) {
        if (k == 0) return std::exchange(t, Tree());
        if (k == size(t.id)) return Tree();
        int id = splay(t.id, k);
        t.id = inner(id).lid;
        return Tree{inner(id).rid, id};
    }
/*    
    template <class F> Tree split_f(Tree& t, F f) {
        if (f(m.e()))
            if (k == 0) return std::exchange(t, Tree());
        if (k == size(t.id)) return Tree();
        int id = splay(t.id, k);
        t.id = inner(id).lid;
        return Tree{inner(id).rid, id};
    }
*/

    void to_vec(int id, std::vector<S>& buf) {
        if (leaf_id(id)) {
            buf.push_back(leaf(id).s);
            return;
        }
        push(id);
        int lid = inner(id).lid, rid = inner(id).rid;
        to_vec(lid, buf);
        to_vec(rid, buf);
    }
    std::vector<S> to_vec(const Tree& t) {
        std::vector<S> buf;
        to_vec(t.id, buf);
        return buf;
    }
};
}