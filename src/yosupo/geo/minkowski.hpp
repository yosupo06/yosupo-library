#pragma once

#include <algorithm>
#include <vector>

#include "yosupo/geo/point.hpp"
#include "yosupo/geo/polygon.hpp"

namespace yosupo {

/**
 * @brief 凸多角形のミンコフスキー和
 * @tparam T 座標型。比較・加減算・乗算を正確に行える符号付き整数型。
 * @param a 反時計回りの凸多角形。空集合・1点・線分も指定できる。
 * @param b a と同じ形式の凸多角形。
 * @return {p + q | p は a 内、q は b 内} を表す凸多角形。
 * 辞書順最小の頂点から反時計回りに並べ、辺上の余分な頂点や
 * 閉じるための末尾の重複点を含めない。どちらかが空なら空を返す。
 * @pre 入力は convex() の出力形式で、重複点・辺上の余分な頂点を
 * 含めない。開始頂点は任意で、線分の2端点の順序も任意。
 * @pre 各入力の頂点数は INT_MAX 未満で、必要な領域を確保できること。
 * @pre 座標の和・差に加え、辺ベクトルの外積の各乗算と減算の
 * 中間結果も T に収まること。広い型が必要なら入力の座標型を広げる。
 * @note 入力を変更しない。同方向の辺はまとめるため、平行な線分同士の
 * 和も余分な頂点を含まない線分となる。
 * @par 計算量
 * 時間・追加空間ともに O(a.size() + b.size())。
 * @par 使用例
 * @code
 * #include <yosupo/geo/convex.hpp>
 * #include <yosupo/geo/minkowski.hpp>
 * auto a = yosupo::convex<long long>({{0, 0}, {2, 0}, {0, 2}});
 * auto b = yosupo::convex<long long>({{0, 0}, {1, 0}});
 * auto sum = yosupo::minkowski_sum(a, b);
 * // {{0, 0}, {3, 0}, {1, 2}, {0, 2}}
 * @endcode
 */
template <class T>
Polygon<T> minkowski_sum(const Polygon<T>& a, const Polygon<T>& b) {
    if (a.empty() || b.empty()) return {};
    const int n = int(a.size()), m = int(b.size());
    auto first_edge = [](const Polygon<T>& p) {
        const int size = int(p.size());
        int first = 0;
        for (int i = 1; i < size; i++) {
            if (cmp_arg(p[(i + 1) % size] - p[i],
                        p[(first + 1) % size] - p[first]) < 0) {
                first = i;
            }
        }
        return first;
    };
    int i = first_edge(a), j = first_edge(b);
    const int na = n == 1 ? 0 : n, nb = m == 1 ? 0 : m;
    int used_a = 0, used_b = 0;
    Polygon<T> result;
    result.reserve(a.size() + b.size());
    result.push_back(a[i] + b[j]);
    while (used_a < na || used_b < nb) {
        bool advance_a = used_a < na, advance_b = used_b < nb;
        if (advance_a && advance_b) {
            auto cmp = cmp_arg(a[(i + 1) % n] - a[i],
                               b[(j + 1) % m] - b[j]);
            advance_a = cmp <= 0;
            advance_b = cmp >= 0;
        }
        if (advance_a) {
            i = (i + 1) % n;
            used_a++;
        }
        if (advance_b) {
            j = (j + 1) % m;
            used_b++;
        }
        if (used_a < na || used_b < nb) result.push_back(a[i] + b[j]);
    }
    std::rotate(result.begin(),
                std::min_element(result.begin(), result.end()), result.end());
    return result;
}

}  // namespace yosupo
