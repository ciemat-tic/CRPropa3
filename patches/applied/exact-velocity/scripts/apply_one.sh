#!/usr/bin/env bash
set -euo pipefail
if [ $# -ne 1 ]; then
  echo "Uso: $0 /ruta/al/patch.patch"
  exit 1
fi
PATCH_FILE="$1"
git apply --reject --whitespace=fix "$PATCH_FILE"
echo "[OK] aplicado $PATCH_FILE"
