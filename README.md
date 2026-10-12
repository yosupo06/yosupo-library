```
git submodule update --init --recursive

oj-verify run test-oj/aplusb.test.cpp
oj-bundle test-oj/simd_convolution.test.cpp -I . -I ac-library/
```

## 特定のojファイルをローカルで検証する

```sh
competitive-verifier oj-resolve --no-bundle --config .verify-helper/config.toml --include test/oj/min_cost_b_flow.test.cpp > verify_files.json
competitive-verifier verify --verify-json verify_files.json                                                     
```

## ミンコフスキー和

`yosupo/geo/minkowski.hpp` の `minkowski_sum(a, b)` は、2つの凸多角形の
ミンコフスキー和を時間・空間計算量 O(|a| + |b|) で求めます。
入力は `convex()` が返す形式（反時計回り、重複点・辺上の余分な頂点なし）です。
開始頂点は任意で、空集合・1点・線分にも対応します。
出力は辞書順最小の頂点から反時計回りで、閉じるための末尾の重複点は含みません。
座標の和・差・外積が座標型に収まる必要があります。

```cpp
#include <yosupo/geo/convex.hpp>
#include <yosupo/geo/minkowski.hpp>

auto a = yosupo::convex<long long>({{0, 0}, {2, 0}, {0, 2}});
auto b = yosupo::convex<long long>({{0, 0}, {1, 0}});
auto sum = yosupo::minkowski_sum(a, b);
// {{0, 0}, {3, 0}, {1, 2}, {0, 2}}
```
