#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace yosupo {

/**
 * @brief ConvexMinCostFlowGraph（凸な区分線形費用の最小費用 b-flow）
 * @tparam Cap 容量・流量・需給の符号付き整数型。int または long long。
 * @tparam Cost 傾き・距離・ポテンシャルの符号付き整数型。int または long long。
 *
 * 各辺 e に整数流量 x[e] と凸な区分線形費用 C_e(x[e]) を持たせ、
 * 0 <= x[e] <= 容量、各頂点の「流出量 - 流入量 = 需給」を満たして
 * sum(C_e(x[e])) を最小化する。C_e(0) = 0 とする。
 * 傾きが非減少の折れ線のみを扱う。負の傾き・負閉路・多重辺・自己ループ・
 * 非連結グラフに対応する。需給の初期値は 0。
 *
 * 残余グラフには各辺について正逆の 2 本だけを持つ。現在の流量 x に対し、
 * 正方向は C_e(x+1)-C_e(x)、逆方向は C_e(x-1)-C_e(x) を費用とし、
 * それぞれ直近の折れ点までを残余容量とする。折れ点の通過時に O(1) で
 * 区間を切り替える。傾きが同じ区間内ではまとめて流せる。
 *
 * @par 共通の利用条件
 * 頂点数 n、元の辺数 m、全区間数 K について、n >= 0、
 * 2 * (m + n) <= INT_MAX、n + 2 <= INT_MAX。
 * 各辺の区間数は INT_MAX 以下。O(n + m + K) の領域を確保できること。
 * 追加は solve() 前に行い、同じオブジェクトの solve() は 1 回のみ。
 *
 * @par 整数範囲
 * 容量の累積和、需給の加算、初期化時の需給補正、および補正後の
 * 正の需給の総和 B と負の需給の絶対値の総和は Cap に収める。
 * 初期化では各辺の負の傾きの区間を満たし、始点の需給から流量を引き、
 * 終点に加える（自己ループでは補正しない）。
 * Cost には傾きの符号反転、c + p[u] - p[v]、距離との加算、
 * p[v] + (dist[v] - dist[t]) の各中間値まで収める。
 * 距離の無限大に Cost の最大値を使うので、有限距離はそれ未満とする。
 * C を全区間の傾きの絶対値の最大（区間がなければ 0）として、
 * 4 * (B + 1) * (n + 2)^2 * (C + 1) < Cost の最大値なら十分。
 * これは保守的な十分条件であり、許容される最大範囲ではない。
 * これらの式は数学的な整数として評価する。総費用は solve() を参照。
 */
template <class Cap, class Cost> struct ConvexMinCostFlowGraph {
    /// 折れ線の 1 区間。cap は区間の長さ、cost は単位流量あたりの費用。
    struct Segment {
        Cap cap;
        Cost cost;
    };

    /**
     * @brief n 頂点・0 辺、全頂点の需給が 0 のグラフを作る。
     * @pre n >= 0。クラスの共通の利用条件を満たすこと。
     * @par 計算量
     * 時間・空間 O(n)。
     */
    explicit ConvexMinCostFlowGraph(int n = 0) : _n(n) {
        assert(n >= 0);
        _excess.resize(n);
    }

    /**
     * @brief 需給 0 の頂点を追加し、追加した頂点番号を返す。
     * @pre solve() 前で、追加後も共通の利用条件を満たすこと。
     * @par 計算量
     * ならし O(1)。
     */
    int add_vertex() {
        assert(!_solved);
        _excess.push_back(0);
        return _n++;
    }

    /**
     * @brief 頂点 v の需給に b を加える。正は供給、負は需要。
     * @pre solve() 前、0 <= v < n。加算途中も Cap に収まること。
     * @note s から t へちょうど F 流すには add_excess(s, F) と
     * add_excess(t, -F) を呼ぶ。需給は下限等の補正をせず指定する。
     * @par 計算量
     * O(1)。
     */
    void add_excess(int v, Cap b) {
        assert(!_solved);
        assert(0 <= v && v < _n);
        _excess[v] += b;
    }

    /**
     * @brief 凸な区分線形費用の有向辺を追加し、0 始まりの辺番号を返す。
     * @param from 始点。
     * @param to 終点。from と同じでもよい。
     * @param segments 流量 0 から順に並べた {区間の長さ, 傾き}。
     * 例えば {{3, 2}, {5, 7}} は容量 8、C(3) = 6、C(8) = 41。
     * 空なら容量 0。全区間をこのオブジェクトが所有する。
     * @pre solve() 前、0 <= from, to < n。各 cap > 0、傾きは非減少。
     * 容量の総和が Cap に収まり、クラスの共通の利用条件を満たすこと。
     * @par 計算量
     * 区間数 k として時間 O(k + 1)（辺配列への追加はならし）、空間 O(k + 1)。
     */
    int add_edge(int from, int to, const std::vector<Segment>& segments) {
        assert(!_solved);
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        Edge e{from, to, {}};
        e.pieces.reserve(segments.size());
        Cap end = 0;
        for (auto s : segments) {
            assert(s.cap > 0);
            assert(s.cap <= std::numeric_limits<Cap>::max() - end);
            assert(e.pieces.empty() || e.pieces.back().cost <= s.cost);
            end += s.cap;
            e.pieces.push_back({end, s.cost});
        }
        int id = int(_edges.size());
        _edges.push_back(std::move(e));
        return id;
    }

    /**
     * @brief 容量 cap、線形費用 C(x) = cost * x の辺を追加する。
     * @return 追加した辺の番号。
     * @pre cap >= 0。他の条件は区分線形費用の add_edge() と同じ。
     * @par 計算量
     * ならし O(1)。
     */
    int add_edge(int from, int to, Cap cap, Cost cost) {
        assert(cap >= 0);
        if (cap == 0) return add_edge(from, to, {});
        return add_edge(from, to, {{cap, cost}});
    }

    /// 求解結果。実行不可能なら {false, 0, {}, {}}。
    template <class TotalCost> struct FlowResult {
        bool feasible;
        TotalCost cost;         ///< 最小総費用。
        std::vector<Cap> flow;  ///< 元の辺の追加順の流量。
        /// 頂点順のポテンシャル p。残余容量のある方向について
        /// C(x+1)-C(x)+p[from]-p[to] >= 0、
        /// C(x-1)-C(x)+p[to]-p[from] >= 0 を満たす。
        std::vector<Cost> potential;
    };

    /**
     * @brief 指定した需給をちょうど満たす最小費用フローを求める。
     * @tparam TotalCost 総費用の符号付き整数型。Cap, Cost 以上の幅を持つ
     * int, long long、または対応するコンパイラの __int128。
     * @return 実行可能性、総費用、元の辺の流量、頂点ポテンシャル。
     * 需給の総和が 0 でない場合も実行不可能を返す。
     * @pre 同じオブジェクトでは 1 回のみ。共通の利用条件を満たすこと。
     * @pre 総費用の各乗算・加算も TotalCost に収まること。
     * 全区間の sum(cap * abs(cost)) が TotalCost の正の最大値以下なら十分。
     * TotalCost を広くしても Cap, Cost の内部演算は広がらない。
     * @note 内部状態を変更する。std::move(g).solve<long long>() のように呼ぶ。
     * 最大流量を求める API ではない。需給が全て 0 なら最小費用循環を求める。
     * @par 計算量
     * 増加路を流す回数を A として、時間
     * O(K + (A + 1) * (n + m + 1) * log(n + m + 2))、空間 O(n + m + K)。
     * 1 回の探索で見る残余辺は O(n + m) であり K に依存しない。
     * A <= B（初期化後の正の需給の総和）。A 自体は区間数・容量に依存し、
     * 強多項式時間ではない。同じ折れ点を往復する場合もある。
     */
    template <class TotalCost> FlowResult<TotalCost> solve() && {
        assert(!_solved);
        _solved = true;
        int m = int(_edges.size());
        int s = _n, t = _n + 1, n = _n + 2;

        // 各辺を単独の最小点に置くと、全残余費用が非負になる。
        for (auto& e : _edges) {
            while (e.next < int(e.pieces.size()) && e.pieces[e.next].cost < 0) {
                e.flow = e.pieces[e.next++].end;
            }
            if (e.from != e.to) {
                _excess[e.from] -= e.flow;
                _excess[e.to] += e.flow;
            }
        }

        Cap supply = 0, demand = 0;
        for (Cap b : _excess) {
            if (b > 0) supply += b;
            if (b < 0) demand -= b;
        }
        if (supply != demand) return {false, 0, {}, {}};
        for (int v = 0; v < _n; v++) {
            if (_excess[v] > 0) {
                _edges.push_back({s, v, {{_excess[v], 0}}});
            } else if (_excess[v] < 0) {
                _edges.push_back({v, t, {{-_excess[v], 0}}});
            }
        }

        std::vector<std::vector<int>> adj(n);
        for (int i = 0; i < int(_edges.size()); i++) {
            adj[_edges[i].from].push_back(2 * i);
            adj[_edges[i].to].push_back(2 * i + 1);
        }
        const Cost inf = std::numeric_limits<Cost>::max();
        std::vector<Cost> p(n), dist(n);
        std::vector<int> prev(n);
        std::vector<bool> visited(n);
        while (supply > 0) {
            std::fill(dist.begin(), dist.end(), inf);
            std::fill(visited.begin(), visited.end(), false);
            using Item = std::pair<Cost, int>;
            std::priority_queue<Item, std::vector<Item>, std::greater<Item>> q;
            dist[s] = 0;
            q.push({0, s});
            while (!q.empty()) {
                auto [d, u] = q.top();
                q.pop();
                if (visited[u]) continue;
                visited[u] = true;
                if (u == t) break;
                for (int id : adj[u]) {
                    const auto& e = _edges[id / 2];
                    bool reverse = bool(id & 1);
                    int v = reverse ? e.from : e.to;
                    auto [cap, cost] = e.residual(reverse);
                    if (cap == 0 || visited[v]) continue;
                    Cost rc = cost + p[u] - p[v];
                    assert(rc >= 0);
                    Cost nd = d + rc;
                    if (nd >= dist[v]) continue;
                    dist[v] = nd;
                    prev[v] = id;
                    q.push({nd, v});
                }
            }
            if (!visited[t]) return {false, 0, {}, {}};
            // 未確定の頂点は dist[t]
            // で打ち切ることで全体の双対実行可能性を保つ。
            for (int v = 0; v < n; v++) {
                if (visited[v]) p[v] += dist[v] - dist[t];
            }

            Cap f = supply;
            for (int v = t; v != s;) {
                int id = prev[v];
                const auto& e = _edges[id / 2];
                f = std::min(f, e.residual(bool(id & 1)).first);
                v = (id & 1) ? e.to : e.from;
            }
            for (int v = t; v != s;) {
                int id = prev[v];
                auto& e = _edges[id / 2];
                e.push(bool(id & 1), f);
                v = (id & 1) ? e.to : e.from;
            }
            supply -= f;
        }

        TotalCost cost = 0;
        std::vector<Cap> flow(m);
        for (int i = 0; i < m; i++) {
            const auto& e = _edges[i];
            flow[i] = e.flow;
            Cap begin = 0;
            for (auto piece : e.pieces) {
                Cap used = std::min(e.flow, piece.end) - begin;
                cost += TotalCost(used) * TotalCost(piece.cost);
                if (e.flow <= piece.end) break;
                begin = piece.end;
            }
        }
        p.resize(_n);
        return {true, cost, std::move(flow), std::move(p)};
    }

  private:
    struct Piece {
        Cap end;
        Cost cost;
    };
    struct Edge {
        int from, to;
        std::vector<Piece> pieces;
        Cap flow = 0;
        int next = 0;  // flow より大きい end を持つ最初の区間。

        std::pair<Cap, Cost> residual(bool reverse) const {
            if (!reverse) {
                if (next == int(pieces.size())) return {0, 0};
                return {pieces[next].end - flow, pieces[next].cost};
            }
            if (flow == 0) return {0, 0};
            int i = next;
            if (i == int(pieces.size()) ||
                (i > 0 && flow == pieces[i - 1].end)) {
                i--;
            }
            Cap begin = i == 0 ? 0 : pieces[i - 1].end;
            return {flow - begin, -pieces[i].cost};
        }

        void push(bool reverse, Cap f) {
            assert(0 < f && f <= residual(reverse).first);
            if (!reverse) {
                flow += f;
                if (flow == pieces[next].end) next++;
            } else {
                if (next == int(pieces.size()) ||
                    (next > 0 && flow == pieces[next - 1].end)) {
                    next--;
                }
                flow -= f;
            }
        }
    };
    int _n;
    bool _solved = false;
    std::vector<Cap> _excess;
    std::vector<Edge> _edges;
};

}  // namespace yosupo
