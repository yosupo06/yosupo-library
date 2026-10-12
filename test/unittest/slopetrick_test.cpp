#include "yosupo/slopetrick.hpp"

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <optional>
#include <random>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace {

using ll = long long;
using ST = yosupo::SlopeTrick<ll>;

TEST(SlopeTrickTest, ConstantAndHinges) {
    ST f(7);
    EXPECT_EQ(f.min(), 7);
    EXPECT_EQ(f.argmin(), (ST::Interval{std::nullopt, std::nullopt}));
    f.add_a_minus_x(2, 3);
    EXPECT_EQ(f.argmin(), (ST::Interval{2, std::nullopt}));
    f.add_x_minus_a(5, 3);
    EXPECT_EQ(f.argmin(), (ST::Interval{2, 5}));
    for (ll x = -10; x <= 10; x++) {
        EXPECT_EQ(f.eval(x),
                  7 + 3 * std::max(2 - x, 0LL) + 3 * std::max(x - 5, 0LL));
    }
    f.add_abs(4, 2);
    EXPECT_EQ(f.argmin(), (ST::Interval{4, 4}));
    EXPECT_EQ(f.min(), 7);
    f.add_const(-10);
    EXPECT_EQ(f.min(), -3);
    f.add_abs(100, 0);
    EXPECT_EQ(f.size(), 3);
}

TEST(SlopeTrickTest, WeightedBreakpointsAndLinear) {
    ST f;
    f.add_abs(-3, 1000000000);
    f.add_abs(5, 1000000000);
    EXPECT_EQ(f.size(), 2);
    EXPECT_EQ(f.min(), 8000000000LL);
    EXPECT_EQ(f.argmin(), (ST::Interval{-3, 5}));
    // Changing the slope back and forth must not expand weights into units.
    for (int i = 0; i < 100; i++) {
        f.add_linear(1);
        EXPECT_EQ(f.argmin(), (ST::Interval{-3, -3}));
        EXPECT_EQ(f.min(), 7999999997LL);
        f.add_linear(-2);
        EXPECT_EQ(f.argmin(), (ST::Interval{5, 5}));
        EXPECT_EQ(f.min(), 7999999995LL);
        f.add_linear(1);
    }
    f.add_abs(-3, 2);
    EXPECT_EQ(f.size(), 2);
    EXPECT_EQ(f.argmin(), (ST::Interval{-3, -3}));
}

TEST(SlopeTrickTest, DomainAndEndpoints) {
    ST f(-5, 8, 10);
    f.add_linear(3, -2);
    EXPECT_EQ(f.min(), -7);
    EXPECT_EQ(f.argmin(), (ST::Interval{-5, -5}));
    f.add_abs(-10, 2);
    f.add_abs(10, 4);
    f.add_abs(0, 3);
    f.restrict_domain(-2, 4);
    EXPECT_EQ(f.domain(), (ST::Interval{-2, 4}));
    EXPECT_FALSE(f.eval(-3));
    EXPECT_FALSE(f.eval(5));
    for (ll x = -2; x <= 4; x++) {
        EXPECT_EQ(f.eval(x),
                  3 * x + 8 + 2 * (x + 10) + 4 * (10 - x) + 3 * std::abs(x));
    }
    EXPECT_EQ(f.argmin(), (ST::Interval{0, 0}));
    f.restrict_domain(1, 1);
    EXPECT_EQ(f.size(), 0);
    EXPECT_EQ(f.min(), 72);
    EXPECT_EQ(f.argmin(), (ST::Interval{1, 1}));
    f.shift_min(-3, 5);
    EXPECT_EQ(f.domain(), (ST::Interval{-2, 6}));
    EXPECT_EQ(f.argmin(), f.domain());
    EXPECT_EQ(f.eval(0), 72);
}

TEST(SlopeTrickTest, PrefixAndSuffixMin) {
    ST f;
    f.add_abs(2, 3);
    f.add_abs(8, 3);
    auto prefix = f;
    auto suffix = f;
    prefix.prefix_min();
    suffix.suffix_min();
    EXPECT_EQ(prefix.argmin(), (ST::Interval{2, std::nullopt}));
    EXPECT_EQ(suffix.argmin(), (ST::Interval{std::nullopt, 8}));
    for (ll x = -10; x <= 20; x++) {
        EXPECT_EQ(prefix.eval(x), f.eval(std::min(x, 2LL)));
        EXPECT_EQ(suffix.eval(x), f.eval(std::max(x, 8LL)));
    }
    prefix.prefix_min();
    suffix.suffix_min();
    prefix.suffix_min();
    suffix.prefix_min();
    EXPECT_EQ(prefix.size(), 0);
    EXPECT_EQ(suffix.size(), 0);
    EXPECT_EQ(prefix.eval(-1000), 18);
    EXPECT_EQ(suffix.eval(1000), 18);

    for (ll slope = -2; slope <= 2; slope++) {
        ST g(-2, 5, 7);
        g.add_linear(slope);
        auto left = g;
        auto right = g;
        left.prefix_min();
        right.suffix_min();
        EXPECT_EQ(left.domain(), (ST::Interval{-2, std::nullopt}));
        EXPECT_EQ(right.domain(), (ST::Interval{std::nullopt, 5}));
        for (ll x = -10; x <= 10; x++) {
            std::optional<ll> a, b;
            for (ll y = -2; y <= 5; y++) {
                if (y <= x && (!a || *g.eval(y) < *a)) a = g.eval(y);
                if (y >= x && (!b || *g.eval(y) < *b)) b = g.eval(y);
            }
            EXPECT_EQ(left.eval(x), a);
            EXPECT_EQ(right.eval(x), b);
        }
    }
}

TEST(SlopeTrickTest, UnboundedLinearFunctions) {
    ST f;
    f.add_linear(3, 7);
    EXPECT_FALSE(f.bounded_below());
    f.suffix_min();
    f.shift_min(-2, 4);
    EXPECT_FALSE(f.bounded_below());
    EXPECT_EQ(f.eval(5), 3 * (5 - 4) + 7);
    f.restrict_domain(0, std::nullopt);
    EXPECT_EQ(f.min(), -5);
    f.prefix_min();
    EXPECT_EQ(f.eval(100), -5);

    ST g;
    g.add_linear(-3, 7);
    g.prefix_min();
    g.shift_min(-2, 4);
    EXPECT_FALSE(g.bounded_below());
    EXPECT_EQ(g.eval(5), -3 * (5 + 2) + 7);
    g.restrict_domain(std::nullopt, 0);
    EXPECT_EQ(g.min(), 1);
    g.suffix_min();
    EXPECT_EQ(g.eval(-100), 1);
}

TEST(SlopeTrickTest, ShiftMinAndLazyQueries) {
    ST f(-20, 30);
    for (ll x = -10; x <= 10; x++) f.add_abs(x, x + 11);
    for (auto [a, b] : std::vector<std::pair<ll, ll>>{
             {-5, -2}, {0, 0}, {3, 7}, {-2, 8}, {5, 5}}) {
        auto old = f;
        f.shift_min(a, b);
        for (ll x = -30; x <= 60; x++) {
            std::optional<ll> best;
            for (ll y = x - b; y <= x - a; y++) {
                auto value = old.eval(y);
                if (value && (!best || *value < *best)) best = value;
            }
            EXPECT_EQ(f.eval(x), best) << x << ' ' << a << ' ' << b;
        }
        EXPECT_EQ(f.min(), old.min());
        auto old_min = old.argmin();
        EXPECT_EQ(f.argmin(),
                  (ST::Interval{*old_min.lower + a, *old_min.upper + b}));
    }
}

TEST(SlopeTrickTest, ShiftMinOnLinesRaysAndIntervals) {
    for (int mask = 0; mask < 4; mask++) {
        for (ll slope = -10; slope <= 3; slope++) {
            ST f(3);
            f.add_linear(slope);
            f.add_x_minus_a(-7);
            f.add_x_minus_a(-2, 2);
            f.add_x_minus_a(3, 3);
            f.add_x_minus_a(9);
            std::optional<ll> l =
                mask & 1 ? std::optional<ll>(-4) : std::nullopt;
            std::optional<ll> r =
                mask & 2 ? std::optional<ll>(6) : std::nullopt;
            f.restrict_domain(l, r);
            for (auto [a, b] : std::vector<std::pair<ll, ll>>{
                     {-5, -2}, {0, 0}, {3, 7}, {-2, 8}}) {
                auto g = f;
                g.shift_min(a, b);
                EXPECT_EQ(g.bounded_below(), f.bounded_below());
                if (f.bounded_below()) {
                    EXPECT_EQ(g.min(), f.min());
                }
                for (ll x = -15; x <= 15; x++) {
                    std::optional<ll> best;
                    for (ll y = x - b; y <= x - a; y++) {
                        if ((l && y < *l) || (r && *r < y)) continue;
                        ll value = 3 + slope * y + std::max(y + 7, 0LL) +
                                   2 * std::max(y + 2, 0LL) +
                                   3 * std::max(y - 3, 0LL) +
                                   std::max(y - 9, 0LL);
                        if (!best || value < *best) best = value;
                    }
                    EXPECT_EQ(g.eval(x), best) << mask << ' ' << slope << ' '
                                               << a << ' ' << b << ' ' << x;
                }
            }
        }
    }
}

TEST(SlopeTrickTest, CopyMoveAndAdd) {
    ST f(-10, 10, -7);
    for (ll x = -5; x <= 5; x++) f.add_abs(x, x + 6);
    f.shift(3);
    ST g(-3, 20, 100);
    g.add_a_minus_x(8, 17);
    g.shift(-2);
    auto sum = f;
    sum.add(g);
    auto swapped = g;
    swapped.add(f);
    for (ll x = -15; x <= 25; x++) {
        auto a = f.eval(x), b = g.eval(x);
        auto expected = a && b ? std::optional<ll>(*a + *b) : std::nullopt;
        EXPECT_EQ(sum.eval(x), expected);
        EXPECT_EQ(swapped.eval(x), expected);
    }
    auto moved = std::move(sum);
    EXPECT_EQ(sum.min(), 0);
    sum.add(std::move(moved));
    EXPECT_EQ(moved.min(), 0);
    ST assigned;
    assigned = sum;
    assigned.add(assigned);
    for (ll x = -5; x <= 13; x++) {
        EXPECT_EQ(assigned.eval(x), 2 * *sum.eval(x));
    }
    assigned = std::move(sum);
    EXPECT_EQ(sum.min(), 0);
    EXPECT_EQ(assigned.min(), swapped.min());
}

TEST(SlopeTrickTest, WideValuesAndNoInfinitySentinel) {
    using Wide = yosupo::SlopeTrick<ll, __int128>;
    Wide f;
    const ll a = 4000000000000000000LL;
    f.add_abs(-a, 10);
    f.add_abs(a, 10);
    EXPECT_TRUE(f.min() == __int128(20) * a);
    EXPECT_EQ(f.argmin().lower, -a);
    EXPECT_EQ(f.argmin().upper, a);
    EXPECT_TRUE(*f.eval(std::numeric_limits<ll>::max()) ==
                __int128(20) * std::numeric_limits<ll>::max());
    EXPECT_TRUE(*f.eval(std::numeric_limits<ll>::min()) ==
                -__int128(20) * std::numeric_limits<ll>::min());
    // Coordinates, too, can be __int128, including in strict C++20 mode.
    yosupo::SlopeTrick<__int128> g;
    g.add_abs(__int128(a) * 10);
    EXPECT_TRUE(g.min() == 0);
    EXPECT_TRUE(*g.argmin().lower == __int128(a) * 10);
}

// An independent finite-domain oracle: explicitly store every integer value.
// All breakpoints and operations are integral, so this also determines the
// continuous piecewise-linear function exactly.
struct Table {
    int lower;
    std::vector<ll> values;
    int upper() const { return lower + int(values.size()) - 1; }
    std::optional<ll> eval(int x) const {
        if (x < lower || upper() < x) return std::nullopt;
        return values[x - lower];
    }
    void restrict_domain(int l, int r) {
        values = std::vector<ll>(values.begin() + l - lower,
                                 values.begin() + r - lower + 1);
        lower = l;
    }
    void shift_min(int a, int b) {
        std::vector<ll> next(values.size() + b - a,
                             std::numeric_limits<ll>::max());
        for (int x = lower + a; x <= upper() + b; x++) {
            for (int y = std::max(lower, x - b); y <= std::min(upper(), x - a);
                 y++) {
                next[x - lower - a] = std::min(next[x - lower - a], *eval(y));
            }
        }
        lower += a;
        values = std::move(next);
    }
};

void check_table(const ST& f, const Table& table) {
    ASSERT_EQ(f.domain(), (ST::Interval{table.lower, table.upper()}));
    for (int x = table.lower - 1; x <= table.upper() + 1; x++) {
        ASSERT_EQ(f.eval(x), table.eval(x)) << "x = " << x;
    }
    ll best = *std::min_element(table.values.begin(), table.values.end());
    int l = table.lower, r = table.upper();
    while (*table.eval(l) != best) l++;
    while (*table.eval(r) != best) r--;
    ASSERT_TRUE(f.bounded_below());
    ASSERT_EQ(f.min(), best);
    ASSERT_EQ(f.argmin(), (ST::Interval{l, r}));
}

TEST(SlopeTrickTest, RandomOperationsAgainstTable) {
    std::mt19937 rng(135);
    auto pick = [&](int l, int r) {
        return std::uniform_int_distribution<int>(l, r)(rng);
    };
    for (int trial = 0; trial < 200; trial++) {
        ST f(-10, 10);
        Table table{-10, std::vector<ll>(21)};
        for (int step = 0; step < 100; step++) {
            int op = pick(0, 10);
            SCOPED_TRACE(::testing::Message() << "trial=" << trial << " step="
                                              << step << " op=" << op);
            if (op <= 3) {
                int a = pick(-20, 20), w = pick(0, 20);
                if (op == 0) f.add_abs(a, w);
                if (op == 1) f.add_a_minus_x(a, w);
                if (op == 2) f.add_x_minus_a(a, w);
                if (op == 3) f.add_linear(a, w);
                for (int x = table.lower; x <= table.upper(); x++) {
                    ll delta = 0;
                    if (op == 0) delta = w * std::abs(x - a);
                    if (op == 1) delta = w * std::max(a - x, 0);
                    if (op == 2) delta = w * std::max(x - a, 0);
                    if (op == 3) delta = a * x + w;
                    table.values[x - table.lower] += delta;
                }
            } else if (op == 4) {
                int l = pick(table.lower, table.upper());
                int r = pick(l, table.upper());
                f.restrict_domain(l, r);
                table.restrict_domain(l, r);
            } else if (op == 5 || op == 6) {
                int a = pick(-5, 5), b = pick(a, 5);
                if (op == 5) {
                    f.shift_min(a, b);
                    table.shift_min(a, b);
                } else {
                    f.shift(a);
                    table.lower += a;
                }
            } else if (op == 7 || op == 8) {
                // Extend the resulting ray by a few points before restricting
                // back to a finite domain for the explicit table oracle.
                if (op == 7) {
                    f.prefix_min();
                    for (int i = 1; i < int(table.values.size()); i++) {
                        table.values[i] =
                            std::min(table.values[i], table.values[i - 1]);
                    }
                    table.values.insert(table.values.end(), 3,
                                        table.values.back());
                } else {
                    f.suffix_min();
                    for (int i = int(table.values.size()) - 2; i >= 0; i--) {
                        table.values[i] =
                            std::min(table.values[i], table.values[i + 1]);
                    }
                    table.values.insert(table.values.begin(), 3,
                                        table.values.front());
                    table.lower -= 3;
                }
                for (int x = table.lower; x <= table.upper(); x++) {
                    ASSERT_EQ(f.eval(x), table.eval(x));
                }
                f.restrict_domain(table.lower, table.upper());
            } else {
                int l = table.lower - pick(0, 3);
                int r = table.upper() + pick(0, 3);
                ST g(l, r);
                Table other{l, std::vector<ll>(r - l + 1)};
                for (int i = 0; i < 5; i++) {
                    int a = pick(-20, 20), w = pick(0, 10);
                    g.add_abs(a, w);
                    for (int x = l; x <= r; x++) {
                        other.values[x - l] += w * std::abs(x - a);
                    }
                }
                for (int x = table.lower; x <= table.upper(); x++) {
                    table.values[x - table.lower] += *other.eval(x);
                }
                if (op == 9) f.add(g);
                if (op == 10) f.add(std::move(g));
            }
            check_table(f, table);
        }
    }
}

TEST(SlopeTrickTest, IsotonicRegression) {
    // min sum w_i*abs(x_i-a_i), subject to x_0 <= ... <= x_{n-1}.
    ST f;
    std::vector<ll> dp(11, 0);
    for (auto [a, w] : std::vector<std::pair<int, int>>{
             {8, 3}, {2, 5}, {7, 1}, {1, 4}, {9, 2}}) {
        f.prefix_min();
        f.add_abs(a, w);
        for (int x = 1; x <= 10; x++) dp[x] = std::min(dp[x], dp[x - 1]);
        for (int x = 0; x <= 10; x++) dp[x] += w * std::abs(x - a);
    }
    EXPECT_EQ(f.min(), *std::min_element(dp.begin(), dp.end()));
}

TEST(SlopeTrickTest, BoundedMovementDP) {
    // Start at x=0. Each step moves by [a,b], pays an absolute-value cost,
    // and must finish inside the prescribed interval.
    ST f(0, 0);
    Table table{0, {0}};
    for (int i = 0; i < 30; i++) {
        int a = i % 3 - 2, b = i % 4 + 1, target = i % 9 - 4;
        f.shift_min(a, b);
        f.add_abs(target, i + 1);
        table.shift_min(a, b);
        for (int x = table.lower; x <= table.upper(); x++) {
            table.values[x - table.lower] += (i + 1) * std::abs(x - target);
        }
        int l = std::max(table.lower, -10), r = std::min(table.upper(), 10);
        f.restrict_domain(l, r);
        table.restrict_domain(l, r);
        check_table(f, table);
    }
}

}  // namespace
