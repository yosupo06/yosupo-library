#!/usr/bin/env bash

set -e

sudo apt-get update

sudo apt-get install -y cmake

# clang
sudo apt-get install -y lsb-release wget software-properties-common gnupg
wget https://apt.llvm.org/llvm.sh -O /tmp/llvm.sh
chmod +x /tmp/llvm.sh
sudo /tmp/llvm.sh 18 all

pip install online-judge-tools online-judge-verify-helper
