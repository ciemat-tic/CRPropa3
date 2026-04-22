#!/usr/bin/env bash
set -euo pipefail
git apply patches/0001-textoutput-precision.patch
git apply patches/0002-textoutput-python-test.patch
echo "[OK] applied TextOutput precision bundle"
