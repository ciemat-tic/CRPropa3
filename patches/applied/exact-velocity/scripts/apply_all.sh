#!/usr/bin/env bash
set -euo pipefail
PATCH_DIR="${1:-patches}"
for p in "$PATCH_DIR"/*.patch; do
  echo "Aplicando $p"
  git apply --reject --whitespace=fix "$p"
done
echo "[OK] todos los parches aplicados"
