// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum
// clang-format on

#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/toptree.hpp"
#include "yosupo/tree.hpp"

using mint = yosupo::ModInt998244353;

inline static yosupo::Scanner sc(stdin);
inline static yosupo::Printer pr(stdout);

struct Path {
    mint a, b, ans, cnt;  // f(x) = ax + b
};
struct PathMonoid {
    using S = Path;
    Path op(Path x, Path y) {
        return Path(x.a * y.a, x.b + x.a * y.b,
                    x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt);
    }
    Path e = Path(mint(1), mint(0), mint(0), mint(0));
};

struct TreeDP {
    struct Point {
        mint ans, cnt;
    };
    struct PointMonoid {
        using S = Point;
        Point op(Point x, Point y) {
            return Point(x.ans + y.ans, x.cnt + y.cnt);
        }
        Point e = Point(mint(0), mint(0));
    };
    PointMonoid point = PointMonoid();

    yosupo::ReversibleMonoid<PathMonoid> path =
        yosupo::ReversibleMonoid(PathMonoid());
    using Path = typename decltype(path)::S;

    struct Vertex {
        mint a, b, val;
    };
    std::vector<Vertex> f;
    Path add_vertex(Point x, int u) {
        x.ans += f[u].val;
        x.cnt += mint(1);
        auto up = PathMonoid::S(f[u].a, f[u].b, f[u].a * x.ans + f[u].b * x.cnt,
                                x.cnt);
        auto down = PathMonoid::S(f[u].a, f[u].b, x.ans, x.cnt);
        return {up, down};
    }
    Point add_edge(Path x) { return Point(x.val.ans, x.val.cnt); }
};

int main() {
    int n, q;
    sc.read(n, q);

    TreeDP dp;
    dp.f = std::vector<TreeDP::Vertex>(n);
    for (int u = 0; u < n; u++) {
        int x;
        sc.read(x);
        dp.f[u].val = x;
    }

    struct E {
        int u, v;
        mint a, b;
    };
    yosupo::RootedTreeBuilder treeb(n);
    std::vector<E> edges(n - 1);
    for (int i = 0; i < n - 1; i++) {
        int u, v, a, b;
        sc.read(u, v, a, b);

        treeb.add_edge(u, v);
        edges[i] = {u, v, a, b};
    }
    auto tree = std::move(treeb).build(0);

    dp.f[0].a = mint(1);
    dp.f[0].b = mint(0);
    for (auto& e : edges) {
        int& u = e.u;
        int& v = e.v;
        if (tree.par[v] == u) std::swap(u, v);
        assert(tree.par[u] == v);
        dp.f[u].a = e.a;
        dp.f[u].b = e.b;
    }
    yosupo::StaticTopTree<TreeDP> tr(tree, dp);

    struct Query {
        int t;
        int u;
        mint a, b;
    };
    std::vector<Query> queries;
    for (int ph = 0; ph < q; ph++) {
        int t;
        sc.read(t);

        if (t == 0) {
            int u, x;
            sc.read(u, x);
            dp.f[u].val = x;
            tr.update(u);
        } else {
            int e, a, b;
            sc.read(e, a, b);
            int u = edges[e].u;
            dp.f[u].a = a;
            dp.f[u].b = b;
            tr.update(u);
        }

        int r;
        sc.read(r);
        auto ans = tr.path_prod(r);
        pr.writeln(ans.rev.ans.val());
    }

    return 0;
}
