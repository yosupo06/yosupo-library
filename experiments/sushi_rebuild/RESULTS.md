# C++ 回転寿司バッチ版の測定

測定日：2026-10-13（日本時間）。

## 条件

- CPU: AMD EPYC 9V45、Linux x86-64 のクラウド環境。
- GCC 14.2.0、C++20。主測定は `-O3 -DNDEBUG`、追加測定は `-O2 -DNDEBUG`。
  `-march=native` や CPU 固有の追加フラグは使っていません。
- 各条件を別プロセスで 3 回、直列に実行。C++ のビルド・単体テストと同時に測っていません。
- 表の時間は CPU 時間の中央値、メモリは 3 回の最大 RSS の最大値です。
  wall time と全サンプルは CSV にあります。RSS は Linux の KiB を MiB に換算します。
- 時間には構築、全クエリ、epoch の再構築、最終配列の復元、回答と配列の checksum、
  データ構造の破棄を含みます。入力生成は時間に含みません。
- RSS には入力クエリ列・回答配列・ドライバ・アロケータの一時的なピークも含みます。
  アロケータと差分の相殺の影響で、間隔と RSS は必ずしも単調な関係になりません。
- 乱数は `std::mt19937_64(12026)`、入力生成は `benchmark.cpp` に保存しています。

| mode | 初期配列 | 区間と投入値 |
| --- | --- | --- |
| `random` | `[0,N+Q)` の乱数 | ランダム区間、投入値も `[0,N+Q)` |
| `increasing` | `[0,N)` のシャッフルした順列 | ランダム区間、投入値 `N+j` |
| `full` | `N-1,...,0` の降順列 | 全区間、投入値 `N+j` |
| `noop` | `[0,N)` のシャッフルした順列 | ランダム区間、投入値 `-1` |

ランダム区間は `[0,N)` から独立に選んだ 2 点を並べ、右端を 1 増やした半開区間です。
`full` は比較の向きを反転させた元問題の「昇順列に小さい値を繰り返し投入」に対応します。

## 結果

既定間隔は `B = max(1, N / (2 * bit_width(N)))`（整数除算）です。
`-O3` での結果を示します。

| N | Q | 入力 | 既定間隔の時間 | 最大 RSS | 半分の間隔の時間 | 最大 RSS |
| ---: | ---: | --- | ---: | ---: | ---: | ---: |
| 400,000 | 25,000 | ランダム投入 | 0.832 秒 | 83.8 MiB | 0.888 秒 | 55.2 MiB |
| 400,000 | 25,000 | 増加する値を投入 | 0.847 秒 | 118.0 MiB | 0.856 秒 | 72.4 MiB |
| 400,000 | 25,000 | 全区間へ増加する値を投入 | 0.546 秒 | 122.5 MiB | 0.625 秒 | 74.2 MiB |
| 200,000 | 200,000 | ランダム投入 | 4.206 秒 | 44.8 MiB | 4.746 秒 | 30.5 MiB |
| 200,000 | 200,000 | 増加する値を投入 | 5.249 秒 | 65.1 MiB | 5.754 秒 | 40.5 MiB |
| 200,000 | 200,000 | 全区間へ増加する値を投入 | 3.792 秒 | 65.1 MiB | 4.355 秒 | 40.8 MiB |

「半分の間隔」は `B = max(1, N / (4 * bit_width(N)))`。
N=400,000 では既定 B=10,526 / 半分 B=5,263、N=200,000 では B=5,555 / B=2,777。
メモリを節約するほど再構築が増えます。さらに大きい・小さい間隔も CSV に保存しています。

`-O2`、N=400,000、Q=25,000 の追加測定：

| 入力 | 既定間隔の時間 | 最大 RSS | 半分の間隔の時間 | 最大 RSS |
| --- | ---: | ---: | ---: | ---: |
| ランダム投入 | 0.896 秒 | 83.8 MiB | 0.893 秒 | 55.2 MiB |
| 増加する値を投入 | 0.848 秒 | 118.0 MiB | 0.882 秒 | 72.4 MiB |
| 全区間へ増加する値を投入 | 0.566 秒 | 122.5 MiB | 0.637 秒 | 74.2 MiB |

生データ：[O3](results_o3.csv) / [O2](results_o2.csv)。
すべての間隔・反復・コンパイラ設定で、各入力の回答と最終配列の checksum が一致しました。

既定値は速度寄りの選択です。特に全区間の入力では、間隔をさらに大きくすると速くなる
場合もありますが、ピークメモリも増えるため、一律に大きくしていません。

## 再現

リポジトリ直下で実行します。測定スクリプトは条件を直列に処理し、反復間で間隔の順序を
反転させます。`--default-only` なら既定間隔のみ、`--size 400k` なら前半のサイズのみです。

```sh
g++ -std=c++20 -O3 -DNDEBUG -Isrc \
  experiments/sushi_rebuild/benchmark.cpp -o /tmp/sushi_benchmark
python3 experiments/sushi_rebuild/measure.py --binary /tmp/sushi_benchmark \
  --output /tmp/sushi-results-o3.csv --repeats 3

g++ -std=c++20 -O2 -DNDEBUG -Isrc \
  experiments/sushi_rebuild/benchmark.cpp -o /tmp/sushi_benchmark_o2
python3 experiments/sushi_rebuild/measure.py --binary /tmp/sushi_benchmark_o2 \
  --output /tmp/sushi-results-o2.csv --repeats 3 --size 400k
```

単発測定は `/tmp/sushi_benchmark N Q mode batch_size`。`batch_size=0` は既定間隔です。
末尾に `naive` を付けると同じ入力の愚直実装を使います（大きい入力には不向きです）。
CSV の列はドライバでは `N,Q,mode,batch_size,method,wall_s,cpu_s,rss_kib,checksum`、
測定スクリプトでは先頭に `repeat,divisor` が付きます。

## 検証

Google Test に 6 テストを追加しました。小配列の全列挙、乱数、重複値、符号付き・
符号なし 64 bit の端点、64 bit ブロック境界、バッチ間の点変更、長い epoch、
非空で符号和がゼロの Trie を含みます。愚直解と約 54.7 万クエリの回答・最終配列を
照合しました。間隔を変えても、測定入力の回答と配列の checksum は一致しています。

全 375 テストを ASan/UBSan 下で実行し、最終の配列復元・API 調整後にも
`SushiTest.*` を再実行しました。LeakSanitizer はこの環境の ptrace 制約で動作しないため、
`ASAN_OPTIONS=detect_leaks=0` を指定しています。ASan と UBSan は有効です。

```sh
cmake -S test -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target unittest
ASAN_OPTIONS=detect_leaks=0 build/unittest --gtest_filter='SushiTest.*'
```

ドキュメントは、ヘッダの最初の `@brief` をローカルで抽出したメタデータを使い、
既存の `competitive-verifier docs` でタイトル・ソース・索引の生成を確認しました。
GitHub Actions の外部 `parse-doxygen` スクリプトそのものは実行していません。

## 計算量と比較の限界

既定間隔では作業領域は `O(N)` words、総時間は償却
`O(N log N + Q log² N)` です。クエリごとの最悪時間保証ではありません。
入力・出力も含めて線形空間にしたい場合、クエリを高々 `N` 件ずつ読み込んで `apply` し、
その都度回答を出力すれば、全 `Q` 件を保存する必要はありません。

元記事・提供元の C++ 実装と、以前の環境で失われた C++ 版は比較に使えていません。
提供された 1.26 秒などの値はハードウェア・入力が揃わないため、速度比を計算できません。
この資料は今回の実装を同じ環境・入力で測った結果です。オンラインジャッジには未提出です。
