# SlopeTrick

```cpp
#include "yosupo/slopetrick.hpp"
yosupo::SlopeTrick<long long> f;  // f(x) = 0
```

下に凸な区分線形関数を持つ。重み付きの絶対値・片側の折れ線の加算、一次関数の加算、累積min、区間min、定義域の制限、関数同士の加算に対応する。

`SlopeTrick<T, V = T>` の `T` は座標と傾き、`V` は関数値の型。どちらも符号付き整数型で、`__int128` も使える。関数は実数上の区分線形関数として扱い、折れ点と有限の定義域端点は整数。整数上の凸DPにもそのまま使える。

```cpp
yosupo::SlopeTrick<long long, __int128> f; // 値だけ128 bit
yosupo::SlopeTrick<long long> g(0, 0);    // x=0で0、それ以外は+∞
yosupo::SlopeTrick<long long> h(-5, 10, 3); // [-5,10]で3
```

定義域は空でない閉区間で、半直線・全実数も扱える。定義域外の値は `+∞` と解釈する。無限端点に数値の番兵は使わず、`std::nullopt` で表す。各操作は自身を変更する。

## 操作

| API | 操作 |
| --- | --- |
| `add_const(c)` | `f(x) += c` |
| `add_linear(a, b = 0)` | `f(x) += a*x + b` |
| `add_abs(a, w = 1)` | `f(x) += w*abs(x-a)` |
| `add_a_minus_x(a, w = 1)` | `f(x) += w*max(a-x, 0)` |
| `add_x_minus_a(a, w = 1)` | `f(x) += w*max(x-a, 0)` |
| `shift(a)` | `f_new(x) = f_old(x-a)`。正の `a` で右へ移動 |
| `shift_min(a, b)` | `f_new(x) = min_{x-b <= y <= x-a} f_old(y)` |
| `prefix_min()` | `f_new(x) = min_{y <= x} f_old(y)` |
| `suffix_min()` | `f_new(x) = min_{y >= x} f_old(y)` |
| `restrict_domain(l, r)` | 定義域を `[l,r]` と交差させる |
| `add(g)` | `f(x) += g(x)`。定義域は交差 |

重み `w` は非負、`shift_min(a,b)` は `a <= b` が必要。
`restrict_domain` の両引数は `optional<T>` なので、`restrict_domain(0, std::nullopt)` で `x >= 0` に制限できる。`restrict_domain` と `add` では定義域の交差が空にならないことが必要。

`prefix_min` は定義域の右端を `+∞`、`suffix_min` は左端を `-∞` にする。`shift_min(a,b)` は定義域 `[l,r]` を `[l+a,r+b]` にする。有限の移動区間とのminを取るので、下に有界でない関数にも使える。

`add(g)` は `g` をコピーして足す。木DPなどで `g` が不要になる場合は `add(std::move(g))` とすればコピーを省ける。内部では折れ点数の小さい方を大きい方に挿入する。コピーは独立しており、move元は全実数上のゼロ関数に戻る。

## 問い合わせ

| API | 戻り値 |
| --- | --- |
| `min()` | 最小値 `V` |
| `argmin()` | 最小値を取る閉区間 `Interval{lower, upper}` |
| `eval(x)` | `optional<V>`。定義域外なら `nullopt` |
| `domain()` | 定義域 `Interval{lower, upper}` |
| `bounded_below()` | 下に有界かどうか |
| `size()` | 定義域の内部にある折れ点数 |

`Interval` の端点は `optional<T>` で、`lower == nullopt` は `-∞`、`upper == nullopt` は `+∞`。例えばゼロ関数の `argmin()` は両方 `nullopt`。単一点の最小点は `lower == upper` となる。

`min()` と `argmin()` は下に有界なときに呼ぶ。`add_linear` の直後などは下に有界とは限らないが、そのまま加算・区間制限を続けてよい。`prefix_min` と `suffix_min` も結果に `-∞` が現れないことが必要。例えば `f(x)=x` に対する `prefix_min()` は不可だが、`suffix_min()` は使える。

## 例: 単調非減少列への重み付きL1回帰

`x_0 <= ... <= x_{n-1}` の制約で `sum w_i*abs(x_i-a_i)` を最小化する。

```cpp
yosupo::SlopeTrick<long long> dp;
for (int i = 0; i < n; i++) {
    dp.prefix_min();       // min_{y <= x} dp(y)
    dp.add_abs(a[i], w[i]);
}
auto answer = dp.min();
auto [left, right] = dp.argmin(); // 最後のxの最適範囲
```

## 例: 移動幅と位置に制約があるDP

初期位置が `0`、各ステップで `a_i <= x_i-x_{i-1} <= b_i`、位置が `[l_i,r_i]`、コストが `w_i*abs(x_i-p_i)`。

```cpp
yosupo::SlopeTrick<long long> dp(0, 0);
for (int i = 0; i < n; i++) {
    dp.shift_min(a[i], b[i]);
    dp.restrict_domain(l[i], r[i]); // 到達可能区間との交差が非空のとき
    dp.add_abs(p[i], w[i]);
}
auto answer = dp.min();
```

定義域の交差が空になる可能性がある場合は、`domain()` で確認して到達不能として処理する。大きな重みの絶対値を足して制約を近似する必要はない。

## 計算量・対応範囲

折れ点をランダム化平衡二分木で持ち、傾きの変化量とその座標との積の和を集約する。同じ座標の折れ点はまとめる。重みの大きさに比例したループやメモリ確保は行わない。

現在の折れ点数を `n` として、二分木を使う操作の計算量は期待値。

| 操作 | 計算量 |
| --- | --- |
| 定数・一次関数の加算、`shift`、`domain`、`bounded_below`、`size` | `O(1)` |
| 絶対値・片側折れ線の加算、`eval`、`min`、`argmin`、`shift_min` | `O(log(n+1))` |
| `restrict_domain`、`prefix_min`、`suffix_min` | `O(log(n+1) + k)`。`k` は削除する折れ点数 |
| コピー | `O(n)` |
| `add(std::move(g))` | `O(m log(n+m+1) + k)`。`m` は加算前の小さい方の折れ点数、`n` は大きい方、`k` は削除する折れ点数 |

各折れ点は一度しか削除されないため、削除コストは操作列全体で償却できる。メモリは生きている折れ点数に比例する。`add(g)` は上記に `g` のコピーのコストが加わる。

型の範囲に収まることは呼び出し側の責任。`T` には座標・遅延移動量・傾き・傾きの変化量の総和、`V` には値だけでなく内部の切片や `sum(座標 * 傾きの変化量)` も収まる必要がある。積は `V` に変換してから計算する。必要に応じて `SlopeTrick<long long, __int128>` を使う。

空の定義域、値が `-∞` の関数、非凸関数、任意の2関数のmin-plus畳み込み、折れ線の削除、最適な経路全体の復元は直接は扱わない。凹関数の最大化DPは符号を反転して使う。
