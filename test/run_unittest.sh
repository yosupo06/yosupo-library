#!/usr/bin/env bash

expected_dir="$(cd "$(dirname "$0")/.." && pwd)"
if [ "$(pwd)" != "$expected_dir" ]; then
  echo "Error: Please run the script from ${expected_dir}."
  exit 1
fi

set -e

mkdir -p .work/build
cmake -S test -B .work/build -DCMAKE_BUILD_TYPE=Debug
cmake --build .work/build
.work/build/unittest
