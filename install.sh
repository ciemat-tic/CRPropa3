#!/usr/bin/env bash
set -euo pipefail

SOURCE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$(cd "${SOURCE_DIR}/.." && pwd)/CRPropa3-data"

PREFIX_INPUT="${1:-${SOURCE_DIR}}"
mkdir -p "${PREFIX_INPUT}"
PREFIX="$(cd "${PREFIX_INPUT}" && pwd)"

BUILD_DIR_INPUT="${2:-${SOURCE_DIR}/build}"
mkdir -p "${BUILD_DIR_INPUT}"
BUILD_DIR="$(cd "${BUILD_DIR_INPUT}" && pwd)"

have_numpy() {
  python3 -c 'import numpy' >/dev/null 2>&1
}

try_load_acme_spack() {
  if [[ -f /etc/profile.d/SPACK2.sh ]]; then
    # SPACK2.sh is written for interactive shells; keep nounset disabled while sourcing it.
    set +u
    source /etc/profile.d/SPACK2.sh >/dev/null 2>&1 || true
    set -u
  fi

  if command -v spack >/dev/null 2>&1; then
    # ACME Rocky9 cascadelake toolchain used by this portable bundle.
    spack load /tjciufg /w6dgctr /3tjlhpm /sqvlzkq /pbvefzl /kxsb4uw >/dev/null 2>&1 || true
  fi
}

require_command() {
  local name="$1"
  if ! command -v "${name}" >/dev/null 2>&1; then
    echo "Falta ${name}" >&2
    return 1
  fi
}

install_if_different() {
  local src="$1"
  local dst="$2"

  if [[ -e "${dst}" && "${src}" -ef "${dst}" ]]; then
    chmod 755 "${dst}"
  else
    install -m 755 "${src}" "${dst}"
  fi
}

if [[ ! -d "${DATA_DIR}/data" ]]; then
  echo "No encuentro el directorio de datos en ${DATA_DIR}/data" >&2
  exit 1
fi

try_load_acme_spack

missing=0
require_command cmake || missing=1
require_command python3 || missing=1
require_command gcc || missing=1
require_command g++ || missing=1
require_command swig || missing=1

if [[ "${missing}" -ne 0 ]]; then
  echo "En ACME prueba primero:" >&2
  echo "  source /etc/profile.d/SPACK2.sh" >&2
  echo "  spack load /tjciufg /w6dgctr /3tjlhpm /sqvlzkq /pbvefzl /kxsb4uw" >&2
  exit 1
fi

if ! have_numpy; then
  echo "Falta numpy en el python3 activo: $(command -v python3)" >&2
  echo "En ACME prueba primero:" >&2
  echo "  source /etc/profile.d/SPACK2.sh" >&2
  echo "  spack load /3tjlhpm" >&2
  exit 1
fi

PY_VER="$(python3 -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')"
PY_INSTALL_DIR="${PREFIX}/lib/python${PY_VER}/site-packages"

echo "Fuente: ${SOURCE_DIR}"
echo "Build:  ${BUILD_DIR}"
echo "Install:${PREFIX}"
echo "Python: $(command -v python3)"
echo "CMake:  $(command -v cmake)"

rm -rf "${BUILD_DIR}/data"
mkdir -p "${BUILD_DIR}/data"
cp -a "${DATA_DIR}/data/." "${BUILD_DIR}/data/"

cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
  -DPython_INSTALL_PACKAGE_DIR="${PY_INSTALL_DIR}" \
  -DDOWNLOAD_DATA=OFF \
  -DBUILD_DOC=OFF \
  -DENABLE_TESTING=OFF \
  -DENABLE_COVERAGE=OFF \
  -DENABLE_GALACTICMAGNETICLENS=OFF \
  -DENABLE_OPENMP=ON \
  -DENABLE_HDF5=ON

cmake --build "${BUILD_DIR}" -j"$(nproc)"
cmake --install "${BUILD_DIR}"

install_if_different "${SOURCE_DIR}/Velocity_test.py" "${PREFIX}/Velocity_test.py"
install_if_different "${SOURCE_DIR}/PropagationBP_test.py" "${PREFIX}/PropagationBP_test.py"

mkdir -p "${PREFIX}/share/crpropa/examples"
install_if_different "${SOURCE_DIR}/Velocity_test.py" "${PREFIX}/share/crpropa/examples/Velocity_test.py"
install_if_different "${SOURCE_DIR}/PropagationBP_test.py" "${PREFIX}/share/crpropa/examples/PropagationBP_test.py"

cat > "${PREFIX}/activate.sh" <<EOF
#!/usr/bin/env bash
export CRPROPA_HOME="${PREFIX}"
export CRPROPA_DATA_PATH="\${CRPROPA_HOME}/share/crpropa"
export LD_LIBRARY_PATH="\${CRPROPA_HOME}/lib:\${LD_LIBRARY_PATH:-}"
export PYTHONPATH="\${CRPROPA_HOME}/lib/python${PY_VER}/site-packages:\${PYTHONPATH:-}"
export PATH="\${CRPROPA_HOME}/bin:\${PATH:-}"
EOF

chmod +x "${PREFIX}/activate.sh"

cat <<EOF
CRPropa instalado en: ${PREFIX}
Compilacion realizada en: ${BUILD_DIR}
Activa el entorno con:
  source "${PREFIX}/activate.sh"
Scripts de prueba:
  cd "${PREFIX}"
  python3 Velocity_test.py
  python3 PropagationBP_test.py
EOF
