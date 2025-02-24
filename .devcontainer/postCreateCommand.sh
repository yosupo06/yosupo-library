#!/usr/bin/env bash

set -e

sudo apt-get update

sudo apt-get install -y cmake

pipx install competitive-verifier online-judge-tools online-judge-verify-helper pre-commit
