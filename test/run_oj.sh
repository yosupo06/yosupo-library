#!/usr/bin/env bash

expected_dir="$(cd "$(dirname "$0")/.." && pwd)"
if [ "$(pwd)" != "$expected_dir" ]; then
  echo "Error: Please run the script from ${expected_dir}."
  exit 1
fi

set -e

competitive-verifier oj-resolve \
    --no-bundle \
    --config .verify-helper/config.toml \
    --include $1 > verify_files.json

competitive-verifier verify --verify-json verify_files.json

rm verify_files.json
