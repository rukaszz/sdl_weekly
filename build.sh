#!/usr/bin/env bash
set -o errexit  # set -e
set -o nounset  # set -u
set -o pipefail

# --- 設定 ---
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOG_DIR="${SCRIPT_DIR}/log"
LOG_FILE="${LOG_DIR}/build.log"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"
CMAKE_EXTRA_ARGS="${CMAKE_EXTRA_ARGS:-}"
RUN_BINARY="../build/sdl_week17"

# --- 色 ---
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
BLUE="\033[0;34m"
RESET="\033[0m"

# --- ヘルパ ---
info() { printf "${BLUE} ==> %s${RESET}\n" "$*"; }
ok() { printf "${GREEN} ✓ %s${RESET}\n" "$*"; }
warn() { printf "${YELLOW} ⚠ %s${RESET}\n" "$*"; }
err() { printf "${RED} × %s${RESET}\n" "$*"; }

# --- 終了時のログ ---
on_error() {
  local rc=$?
  if [ $rc -ne 0 ]; then
    err "Build failed(exit ${rc}). Display log file tail: ${LOG_FILE}"
    echo "--- last 200 lines of ${LOG_FILE} ---"
    tail -n 200 "${LOG_FILE}" || true
  fi
  exit $rc    
}
trap on_error ERR

# --- 引数処理： "--clean" を引数で渡すとビルドディレクトリを削除して再構成 ---
CLEAN=0
for arg in "$@"; do
  case "$arg" in
    --clean) CLEAN=1 ;;
    --help|-h)
      cat <<EOF
Usage: $0 [--clean] [--] [cmake args ...]
   --clean : remove build directory before configuring
   JOBS=N  : set parallel jobs via env var
   CMAKE_EXTRA_ARGS : extra args passed to cmake via env
EOF
      exit 0
      ;;
  esac
done

# --- 準備 ---
info "build dir: ${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
: > "${LOG_FILE}"

if [ "${CLEAN}" -eq 1 ]; then
  warn "clean build: ${BUILD_DIR} will delete"
  rm -rf "${BUILD_DIR}"
  mkdir -p "${BUILD_DIR}"
fi

# --- ジェネレータ判定 ---
GENERATOR=""
if command -v ninja >/dev/null 2>&1; then
  GENERATOR="ninja"
  info "Ninja is found. Build with Ninja"
else
  GENERATOR="Unix Makefiles"
  info "Ninja is not found. Build with Make"
fi

# --- cmake 実行 ---
info "execute cmake: (generator: ${GENERATOR})"
pushd "${BUILD_DIR}" >/dev/null
{
  printf "\n --- cmake configure: %s ---\n" "$(date)"
  cmake -G "${GENERATOR}" ${CMAKE_EXTRA_ARGS} .. 2>&1
} >> "${LOG_FILE}" 2>&1
ok "cmake configure complete"

# --- make実行 ---
info "Build start: jobs on parallel ${JOBS}"
if [ "${GENERATOR}" = "Ninja" ]; then
  {
    printf "\n--- ninja build: %s ---\n" "$date"
    ninja -j "${JOBS}"
  } >> "${LOG_FILE}" 2>&1
fi
ok "build complete"

popd >/dev/null

# --- 実行ファイルの実行 ---
if [ -x "${RUN_BINARY}" ]; then
  info "Out file execute: ${RUN_BINARY}"
  printf "\n--- run binary: %s ---\n" "$(date)" >> "${LOG_FILE}"
  "${RUN_BINARY}" 2>&1 | tee -a "${LOG_FILE}"
  ok "execute complete"
else
  warn "execute file is not found or unauthorized: ${RUN_BINARY}"
  warn "make sure Bulid log: ${LOG_FILE}"
  exit 1
fi

# --- 正常終了 ---
ok "All processing finished"
exit 0
