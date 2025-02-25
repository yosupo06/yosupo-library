#!/usr/bin/env bash

set -e

sudo apt-get update

# Install GNU time
sudo apt-get install -y time

pipx install competitive-verifier online-judge-tools online-judge-verify-helper pre-commit
pre-commit install
