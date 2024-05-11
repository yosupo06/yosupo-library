#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "atcoder/internal_queue"

namespace yosupo {
struct MaxMatching {
    MaxMatching(int _n)
        : n(_n), g(_n), mate(n, -1) {}

    void add_edge(int a, int b) {
        g[a].push_back(b);
        g[b].push_back(a);
    }

    std::vector<std::pair<int, int>> max_matching() {
        greedy_match();
        blossom_match();
        std::vector<std::pair<int, int>> result;
        for (int i = 0; i < n; i++) {
            if (i < mate[i]) {
                result.push_back({i, mate[i]});
            }
        }
        return result;
    }

  private:
    int n;
    std::vector<std::vector<int>> g;

    std::vector<int> mate; // (i, mate[i]) はマッチしている, もしくは mate[i] = -1

    void greedy_match() {
        for (int i = 0; i < n; i++) {
            for (int j : g[i]) {
                if (mate[i] == -1 && mate[j] == -1) {
                    mate[i] = mate[j];
                    mate[j] = mate[i];
                }
            }
        }
    }

    void blossom_match() {
        std::vector<int> is_even(n, -1); // (i -> is_even[i])への交互パスが存在
        std::vector<int> _first(n); // 親方向
        
        // 代表偶点 nx[i] = (j, -1) : P(i) = (i, mate[i]) + (mate[i], j) + P(j)
        // 非代表偶点 nx[i] = (j, k) : P(i) = P(j) + P(k) + 辺(j, k) - P(mate[i]) + (i, mate[i])
        std::vector<std::pair<int, int>> nx(n);

        auto match = [&](auto self, int p, int b) {
            int d = mate[p];
            mate[p] = b;
            if (d == -1 || mate[d] != p) return;
            if (nx[p].second == -1) {
                mate[d] = nx[p].first;
                self(self, nx[p].first, d);
            } else {
                self(self, nx[p].first, nx[p].second);
                self(self, nx[p].second, nx[p].first);
            }
        };

        // st を支点にM-増加パスを探索
        auto argment = [&](int st) {
            // 親の奇点を返す
            auto first_rec = [&](auto self, int x) {
                if (_first[x] == -1 || is_even[_first[x]] != st)
                    return _first[x];
                return _first[x] = self(self, _first[x]);
            };
            auto first = [&](int x) {
                return first_rec(first_rec, x);
            };

            is_even[st] = st;
            _first[st] = -1;
            nx[st] = {-1, -1};
            atcoder::internal::simple_queue<int> q;
            q.push(st);
            while (q.size()) {
                int a = q.front();
                q.pop();
                for (int b : g[a]) {
                    if (b == st) continue;
                    if (mate[b] == -1) {
                        mate[b] = a;
                        match(match, a, b);
                        return true;
                    }
                    if (is_even[b] == st) {
                        int x = first(a), y = first(b);
                        if (x == y) continue;
                        int z = -1;
                        while (x != -1 || y != -1) {
                            if (y != -1) std::swap(x, y);
                            if (nx[x] == std::make_pair(a, b)) {
                                z = x;
                                break;
                            }
                            nx[x] = {a, b};
                            x = first(nx[mate[x]].first);
                        }
                        for (int v : {first(a), first(b)}) {
                            while (v != z) {
                                q.push(v);
                                is_even[v] = st;
                                _first[v] = z;
                                v = first(nx[mate[v]].first);
                            }
                        }
                    } else if (is_even[mate[b]] != st) {
                        is_even[mate[b]] = st;
                        nx[b] = {-1, -1};
                        nx[mate[b]] = {a, -1};
                        _first[mate[b]] = b;
                        q.push(mate[b]);
                    }
                }
            }
            return false;
        };

        for (int st = 0; st < n; st++) {
            if (mate[st] == -1) argment(st);
        }        
    }
};

}  // namespace yosupo
