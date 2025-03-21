// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum_fixed_root
// clang-format on

#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/fastio.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/toptree.hpp"
#include "yosupo/tree.hpp"

using mint = yosupo::ModInt998244353;

inline static yosupo::Scanner sc(stdin);
inline static yosupo::Printer pr(stdout);

struct TreeDP {
    struct Point {
        mint ans, cnt;
    };
    struct Path {
        mint a, b, ans, cnt;  // f(x) = ax + b
    };

    struct PointMonoid {
        using S = Point;
        Point op(Point x, Point y) {
            return Point(x.ans + y.ans, x.cnt + y.cnt);
        }
        Point e = Point(mint(0), mint(0));
    };
    PointMonoid point = PointMonoid();

    struct PathMonoid {
        using S = Path;
        Path op(Path x, Path y) {
            return Path(x.a * y.a, x.b + x.a * y.b,
                        x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt);
        }
        Path e = Path(mint(1), mint(0), mint(0), mint(0));
    };
    PathMonoid path = PathMonoid();

    struct Vertex {
        mint a, b;
        mint val;
    };
    Path add_vertex(Point x, Vertex f) {
        x.ans += f.val;
        x.cnt += mint(1);
        return Path(f.a, f.b, f.a * x.ans + f.b * x.cnt, x.cnt);
    }
    Point add_edge(Path x) { return Point(x.ans, x.cnt); }
};

int main() {
    int n, q;
    sc.read(n, q);

    std::vector<TreeDP::Vertex> f(n);
    for (int u = 0; u < n; u++) {
        int x;
        sc.read(x);
        f[u].val = x;
    }

    struct E {
        int u, v;
        mint a, b;
    };
    std::vector<E> edges(n - 1);
    yosupo::RootedTreeBuilder treeb(n);
    for (int i = 0; i < n - 1; i++) {
        int u, v, a, b;
        sc.read(u, v, a, b);

        treeb.add_edge(u, v);
        edges[i] = {u, v, a, b};
    }
    auto tree = std::move(treeb).build(0);

    f[0].a = mint(1);
    f[0].b = mint(0);
    for (auto& e : edges) {
        int& u = e.u;
        int& v = e.v;
        if (tree.par[v] == u) std::swap(u, v);
        assert(tree.par[u] == v);
        f[u].a = e.a;
        f[u].b = e.b;
    }
    yosupo::StaticTopTree<TreeDP> top_tree(tree, std::move(f));

    struct Query {
        int t;
        int u;
        mint a, b;
    };
    std::vector<Query> queries;
    for (int ph = 0; ph < q; ph++) {
        int t;
        int u;
        sc.read(t);

        if (t == 0) {
            int x;
            sc.read(u, x);
            auto fu = top_tree.get_vertex(u);
            fu.val = x;
            top_tree.update(u, fu);
        } else {
            int e, a, b;
            sc.read(e, a, b);
            u = edges[e].u;
            auto fu = top_tree.get_vertex(u);
            fu.a = a;
            fu.b = b;
            top_tree.update(u, fu);
        }
        pr.writeln(top_tree.all_prod().ans.val());
    }

    return 0;
}