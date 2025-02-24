#!/usr/bin/env bash

set -e

pipx install competitive-verifier online-judge-tools online-judge-verify-helper pre-commit
pre-commit install
