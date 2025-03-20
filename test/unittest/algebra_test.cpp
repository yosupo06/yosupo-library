#include "yosupo/algebra.hpp"
#include <vector>
#include "yosupo/modint.hpp"

using namespace yosupo;

static_assert(monoid<Max<int>>);
static_assert(monoid<Min<int>>);
static_assert(monoid<Sum<int>>);
static_assert(monoid<Prod<int>>);

using mint = ModInt<998244353>;

struct ModAffine {
    struct S {
        // f(x) = a * x + b
        mint a, b;
    };

    S op(S x, S y) { return S(x.a * y.a, x.b + x.a * y.b); }
    S e() { return S(mint(1), mint(0)); }
};

struct ABC351G {
    using Point = mint;
    using Path = ModAffine::S;
    Prod<Point> point = Prod<Point>(mint(1));
    ModAffine path = ModAffine();

    std::vector<mint> a;
    Path add_vertex(Point x, int u) { return Path(x, a[u]); }
    Point add_edge(Path x) { return Point(x.a + x.b); }
};
static_assert(static_top_tree_dp<ABC351G>);

struct PointSetTreePathCompositeSumFixedRoot {
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
        Point e() { return Point(mint(0), mint(0)); }
    };
    PointMonoid point = PointMonoid();

    struct PathMonoid {
        using S = Path;
        Path op(Path x, Path y) {
            return Path(x.a * y.a, x.b + x.a * y.b,
                        x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt);
        }
        Path e() { return Path(mint(1), mint(0), mint(0), mint(0)); }
    };
    PathMonoid path = PathMonoid();

    struct Vertex {
        mint a, b;
        mint val;
    };
    std::vector<Vertex> f;
    Path add_vertex(Point x, int u) {
        x.ans += f[u].val;
        x.cnt += mint(1);
        return Path(f[u].a, f[u].b, f[u].a * x.ans + f[u].b * x.cnt, x.cnt);
    }
    Point add_edge(Path x) { return Point(x.ans, x.cnt); }
};
static_assert(static_top_tree_dp<PointSetTreePathCompositeSumFixedRoot>);
