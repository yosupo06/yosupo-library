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
