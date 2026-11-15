#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <ranges>
#include <utility>
#include <vector>

namespace yosupo {

/**
 * @brief NetworkSimplexGraph（下限・上限付き最小費用 b-flow）
 * @tparam Cap 下限・上限・流量・需給の型。int または long long。
 * @tparam Cost 辺の費用・ポテンシャルの型。int または long long。
 *
 * 各辺 e の整数流量 f[e] について lower[e] <= f[e] <= upper[e] と、
 * 各頂点 v の「流出量 - 流入量 = b[v]」を満たし、sum(cost[e] * f[e]) を
 * 最小化する。b[v] は add_excess(v, ex) で指定した値の合計で、初期値は 0。
 * 正の b[v] は供給、負の b[v] は需要を表す。
 *
 * 多重辺・自己ループ・非連結グラフ・負の費用・負閉路を扱える。
 * 下限・上限も負でよく、負の流量は辺の向きと逆向きの流量を表す。
 * 下限・上限は有限の整数で、無限容量を表す特別値はない。
 * 全頂点の b[v] が 0 の場合は最小費用循環を求める。
 *
 * @par 共通の利用条件
 * 頂点数 n、追加した辺数 m について、n >= 0、2 * (m + n) <= INT_MAX、
 * 2 * (n + 1) <= INT_MAX を満たし、O(n + m) の領域を確保できること。
 * 頂点・辺・需給の追加は solve() の呼び出し前に行う。
 * 公開されている内部状態を直接書き換えないこと。
 *
 * @par 整数範囲（オーバーフローを避ける十分条件）
 * M_cap, M_cost をそれぞれ Cap, Cost の正の最大値とする。
 * - 各辺で 0 <= upper - lower <= M_cap を満たす。
 * - 構築中の内部の excess[v] を、各加減算の直後に [-M_cap, M_cap] に
 *   収める。辺追加時は始点から初期フローを引き、終点に加える。
 *   自己ループでも減算と加算は別々に行う。solve() で符号を反転するため、
 *   最終的な excess[v] に Cap の最小値を使うことはできない。
 * - 全辺の絶対費用の和 S = sum(abs(cost[e])) について
 *   4 * S + 2 <= M_cost を満たす。容量が 0 の辺の費用も S に含める。
 *   内部で 1 + S と被約費用を Cost 型で計算するための保守的な条件であり、
 *   許容される入力の最大範囲ではない。
 *
 * 上記の式は数学的な整数として評価する。チェック用の式自体も
 * オーバーフローさせないこと。総費用の型・範囲の条件は solve() を参照。
 * TotalCost を広くしても、内部の Cap, Cost の演算は広がらない。
 */
template <class Cap, class Cost> struct NetworkSimplexGraph {
    /// 内部の残余辺。元の辺の最終流量は FlowResult::flow で取得する。
    struct Edge {
        int to;
        Cap cap;
        Cost cost;
    };
    std::vector<Edge> edges;  ///< 残余辺を正方向・逆方向の順に保持する。
    std::vector<Cap> lowers;  ///< 元の辺の下限。辺の追加順。

    int n;
    std::vector<Cap> excess;  ///< 初期フローによる補正後の内部の需給。

    /**
     * @brief _n 頂点・0 辺のグラフを作る。各頂点の需給は 0。
     * @pre _n >= 0。クラスの共通の利用条件を満たすこと。
     * @par 計算量
     * 時間・空間ともに O(_n)。
     */
    explicit NetworkSimplexGraph(int _n = 0) : n(_n), excess(n) {}

    /**
     * @brief 需給 0 の頂点を 1 つ追加する。
     * @return 追加した頂点の番号（追加前の n）。
     * @pre 追加後もクラスの共通の利用条件を満たすこと。
     * @par 計算量
     * ならし O(1)。
     */
    int add_vertex() {
        n++;
        excess.push_back(0);
        return n - 1;
    }

    /**
     * @brief 頂点 i の需給に ex を加算する。上書きではない。
     * @param i 頂点番号。
     * @param ex 追加する需給。正なら供給、負なら需要。
     * @pre 0 <= i < n。加算後の excess[i] がクラスの整数範囲を満たすこと。
     * @note 下限・初期フローによる補正は内部で行うため、元の問題の需給を
     * 指定する。s から t へちょうど F 流すなら、他に需給がなければ
     * add_excess(s, F), add_excess(t, -F) とする。
     * @par 計算量
     * O(1)。
     */
    void add_excess(int i, Cap ex) { excess[i] += ex; }

    /**
     * @brief 下限・上限付きの有向辺を追加する。戻り値はない。
     * @param u 始点。
     * @param v 終点。u と同じでもよい。
     * @param lower 流量の下限。
     * @param upper 流量の上限。lower と等しければ流量は固定される。
     * @param cost 単位流量あたりの費用。
     * @pre 0 <= u, v < n、lower <= upper。
     * @pre クラスの整数範囲を満たすこと。初期フロー lower により
     * excess[u] -= lower, excess[v] += lower の順に補正する。
     * @note 最終流量は、2 種類の辺追加関数を合わせた追加順に
     * FlowResult::flow に格納される。
     * @par 計算量
     * ならし O(1)。
     */
    void add_edge(int u, int v, Cap lower, Cap upper, Cost cost) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        edges.push_back({v, upper - lower, cost});
        edges.push_back({u, 0, -cost});
        excess[u] -= lower;
        excess[v] += lower;
        lowers.push_back(lower);
    }

    /**
     * @brief 初期フローを指定して有向辺を追加する。
     * @param u 始点。
     * @param v 終点。u と同じでもよい。
     * @param lower 流量の下限。
     * @param upper 流量の上限。
     * @param cost 単位流量あたりの費用。
     * @param flow 初期フロー。最終流量を固定する値ではない。
     * @pre 0 <= u, v < n、lower <= flow <= upper。
     * @pre クラスの整数範囲を満たすこと。需給の補正には lower の代わりに
     * flow を使い、excess[u] -= flow, excess[v] += flow の順に補正する。
     * @note 初期フロー全体が需給の条件を満たす必要はない。
     * add_edge() と同じ問題を解き、実行可能性・最小費用は変わらない。
     * 実行時間や選ばれる最適解は変わることがある。
     * flow == lower の場合は add_edge() と同じ。
     * @par 計算量
     * ならし O(1)。
     */
    void add_edge_with_flow(int u,
                            int v,
                            Cap lower,
                            Cap upper,
                            Cost cost,
                            Cap flow) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        edges.push_back({v, upper - flow, cost});
        edges.push_back({u, flow - lower, -cost});
        excess[u] -= flow;
        excess[v] += flow;
        lowers.push_back(lower);
    }

    /// 最小費用 b-flow の結果。まず feasible を確認すること。
    template <class TotalCost> struct FlowResult {
        bool feasible;  ///< 制約を満たすフローが存在するか。
        TotalCost cost;  ///< 最小総費用。下限の分も含む。実行不可能なら 0。
        /// 辺の追加順の最終流量（下限からの増分ではない）。
        /// 実行可能なら長さ m、実行不可能なら空。
        std::vector<Cap> flow;
        /**
         * 頂点順のポテンシャル p。実行可能なら長さ n、実行不可能なら空。
         * 各辺 e = (u, v) の被約費用 rc = cost[e] + p[u] - p[v] は、
         * flow[e] < upper[e] なら rc >= 0、
         * flow[e] > lower[e] なら rc <= 0 を満たす。
         * 特定の頂点の p が 0 になる保証はない。
         */
        std::vector<Cost> potential;
    };

    /**
     * @brief 指定した需給・下限・上限を満たす最小費用フローを求める。
     * @tparam TotalCost 総費用の符号付き整数型。int, long long、または
     * 対応するコンパイラでは __int128。Cap, Cost 以上のビット幅を使う。
     * @return 実行可能なら最適解とポテンシャル。最適解が複数ある場合、
     * どれを返すかは指定されない。実行不可能なら {false, 0, {}, {}}。
     * @pre クラスの共通の利用条件・整数範囲を満たすこと。
     * @pre 総費用の各乗算・加算途中も TotalCost に収まること。
     * 数学的な整数として sum(max(abs(lower[e]), abs(upper[e])) *
     * abs(cost[e])) <= TotalCost の正の最大値なら十分。
     * @pre 同じオブジェクトでの呼び出しは 1 回のみ。
     * @note 内部状態を変更する。呼び出し後のグラフへの追加・再求解は
     * サポートしない。名前付き変数には std::move(g).solve<TotalCost>()
     * と書く。TotalCost は省略できない。
     * @note 需給の総和が 0 でない場合も渡せるが、実行不可能となる。
     * 流量自体を最大化する API ではなく、指定した需給をちょうど満たす。
     * @par 計算量
     * ピボット回数を P として O((P + 1) * (n + m + 1))、空間 O(n + m)。
     * P は入力に依存し、n, m のみで表す多項式時間の保証は設けない。
     */
    template <class TotalCost> FlowResult<TotalCost> solve() && {
        int m = int(edges.size()) / 2;

        // potential
        std::vector<Cost> p(n + 1);

        Cost art_cost = 1;
        for (int i = 0; i < m; i++) {
            art_cost += std::abs(edges[2 * i].cost);
        }

        struct Parent {
            int p;
            int e;  // edge id of (i -> p)
            Cap up, down;
        };
        std::vector<Parent> parents(n);

        edges.reserve(2 * (m + n));
        for (int i = 0; i < n; i++) {
            if (excess[i] >= 0) {
                edges.push_back({n, 0, art_cost});
                edges.push_back({i, excess[i], -art_cost});
                p[i] = -art_cost;
            } else {
                edges.push_back({n, -excess[i], 0});
                edges.push_back({i, 0, 0});
                p[i] = 0;
            }
            parents[i].p = n;
            parents[i].e = int(edges.size()) - 2;
            parents[i].up = edges[parents[i].e].cap;
            parents[i].down = edges[parents[i].e ^ 1].cap;
        }

        std::vector<int> depth(n + 1, 1);
        depth[n] = 0;

        std::vector<int> next(2 * (n + 1)), prev(2 * (n + 1));
        auto conn = [&](int a, int b) {
            next[a] = b;
            prev[b] = a;
        };
        for (int i : std::views::iota(0, n + 1)) {
            conn(2 * i, 2 * i + 1);
        }
        for (int i : std::views::iota(0, n)) {
            conn(2 * i + 1, next[2 * n]);
            conn(2 * n, 2 * i);
        }

        auto push_flow = [&](int ei0) {
            const int u0 = edges[ei0 ^ 1].to, v0 = edges[ei0].to;
            const Cost cycle_len = edges[ei0].cost + p[u0] - p[v0];

            Cap flow = edges[ei0].cap;
            bool del_u_side = true;
            int del_u = v0;

            int lca = -1;
            {
                int u = u0, v = v0;
                auto up_u = [&]() {
                    if (parents[u].down < flow) {
                        flow = parents[u].down;
                        del_u = u;
                        del_u_side = true;
                    }
                    u = parents[u].p;
                };
                auto up_v = [&]() {
                    if (parents[v].up <= flow) {
                        flow = parents[v].up;
                        del_u = v;
                        del_u_side = false;
                    }
                    v = parents[v].p;
                };

                if (depth[u] >= depth[v]) {
                    int cnt = depth[u] - depth[v];
                    for (int i = 0; i < cnt; i++) {
                        up_u();
                    }
                } else {
                    int cnt = depth[v] - depth[u];
                    for (int i = 0; i < cnt; i++) {
                        up_v();
                    }
                }
                while (u != v) {
                    up_u();
                    up_v();
                }
                lca = u;
            }

            if (flow) {
                int u = u0;
                while (u != lca) {
                    parents[u].up += flow;
                    parents[u].down -= flow;
                    u = parents[u].p;
                }
                int v = v0;
                while (v != lca) {
                    parents[v].up -= flow;
                    parents[v].down += flow;
                    v = parents[v].p;
                }
            }

            int u = u0, par = v0;
            Cost p_diff = -cycle_len;
            std::pair<Cap, Cap> p_caps = {edges[ei0].cap - flow,
                                          edges[ei0 ^ 1].cap + flow};
            if (!del_u_side) {
                std::swap(u, par);
                std::swap(p_caps.first, p_caps.second);
                p_diff *= -1;
            }
            int par_e = ei0 ^ (del_u_side ? 0 : 1);

            while (par != del_u) {
                int d = depth[par];
                int idx = 2 * u;
                while (idx != 2 * u + 1) {
                    if (idx % 2 == 0) {
                        d++;
                        p[idx / 2] += p_diff;
                        depth[idx / 2] = d;
                    } else {
                        d--;
                    }
                    idx = next[idx];
                }

                conn(prev[2 * u], next[2 * u + 1]);
                conn(2 * u + 1, next[2 * par]);
                conn(2 * par, 2 * u);

                std::swap(parents[u].e, par_e);
                par_e ^= 1;

                std::swap(parents[u].up, p_caps.first);
                std::swap(parents[u].down, p_caps.second);
                std::swap(p_caps.first, p_caps.second);

                int next_u = parents[u].p;
                parents[u].p = par;
                par = u;
                u = next_u;
            }
            edges[par_e].cap = p_caps.first;
            edges[par_e ^ 1].cap = p_caps.second;
        };

        // pivot-rule: candidate-list
        const int LIST_LENGTH = std::max(int(0.3 * std::sqrt(double(m))), 10);
        const int MINOR_LIMIT = std::max(int(0.1 * LIST_LENGTH), 3);
        std::vector<int> candidates;
        candidates.reserve(LIST_LENGTH);

        auto minor = [&]() {
            if (candidates.empty()) return false;
            Cost best = 0;
            int best_ei = -1;

            int i = 0;
            while (i < int(candidates.size())) {
                int ei = candidates[i];
                if (!edges[ei].cap) {
                    std::swap(candidates[i], candidates.back());
                    candidates.pop_back();
                    continue;
                }
                Cost c_len =
                    edges[ei].cost + p[edges[ei ^ 1].to] - p[edges[ei].to];
                if (c_len >= 0) {
                    std::swap(candidates[i], candidates.back());
                    candidates.pop_back();
                    continue;
                }
                if (c_len < best) {
                    best = c_len;
                    best_ei = ei;
                }
                i++;
            }

            if (best_ei == -1) return false;
            push_flow(best_ei);
            return true;
        };

        int ei = 0;
        while (true) {
            for (int i = 0; i < MINOR_LIMIT; i++) {
                if (!minor()) break;
            }

            Cost best = 0;
            int best_ei = -1;

            candidates.clear();
            for (int i = 0; i < m + n; i++) {
                Cost clen = edges[2 * ei].cost + p[edges[2 * ei + 1].to] -
                            p[edges[2 * ei].to];
                if (clen < 0) {
                    if (edges[2 * ei].cap) {
                        if (clen < best) {
                            best = clen;
                            best_ei = 2 * ei;
                        }
                        candidates.push_back(2 * ei);
                        if (int(candidates.size()) == LIST_LENGTH) break;
                    }
                } else if (-clen < 0) {
                    if (edges[2 * ei + 1].cap) {
                        if (-clen < best) {
                            best = -clen;
                            best_ei = 2 * ei + 1;
                        }
                        candidates.push_back(2 * ei + 1);
                        if (int(candidates.size()) == LIST_LENGTH) break;
                    }
                }
                ei++;
                if (ei == m + n) ei = 0;
            }
            if (candidates.empty()) break;
            push_flow(best_ei);
        }

        for (int i : std::views::iota(0, n)) {
            edges[parents[i].e].cap = parents[i].up;
            edges[parents[i].e ^ 1].cap = parents[i].down;
        }

        bool feasible = true;
        for (int i : std::views::iota(0, n)) {
            if (excess[i] >= 0) {
                if (edges[2 * (m + i) + 1].cap) {
                    feasible = false;
                    break;
                }
            } else {
                if (edges[2 * (m + i)].cap) {
                    feasible = false;
                    break;
                }
            }
        }
        if (!feasible) {
            return {false, {}, {}, {}};
        }

        TotalCost cost = 0;
        std::vector<Cap> flow;
        for (int i = 0; i < m; i++) {
            flow.push_back(lowers[i] + edges[(2 * i) + 1].cap);
            cost += TotalCost(1) * flow.back() * edges[2 * i].cost;
        }
        p.pop_back();
        return {true, cost, flow, p};
    }
};

}  // namespace yosupo
