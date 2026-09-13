#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
build="$root/build"
config="Release"
clean=0
tests=0
run=0
shared=0
install=0
install_prefix="${RIBBONLIB_INSTALL_PREFIX:-$build/install}"
jobs="${BUILD_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}"

while [[ $# -gt 0 ]]; do
    case "$1" in
        Debug|Release) config="$1"; shift ;;
        --clean|-Clean) clean=1; shift ;;
        --tests|-Tests) tests=1; shift ;;
        --run|-Run) run=1; shift ;;
        --shared|-Shared) shared=1; shift ;;
        --install|-Install) install=1; shift ;;
        --prefix) install_prefix="$2"; shift 2 ;;
        -j|--jobs) jobs="$2"; shift 2 ;;
        -h|--help)
            cat <<'EOF'
RibbonLib build helper
Usage: ./build.sh [Debug|Release] [options]

Options:
  --clean           Remove build directory first
  --tests           Build and run smoke tests
  --run             Run the example after build
  --shared          Build RibbonLib as a shared library
  --install         Install after build
  --prefix DIR      Install prefix (default: build/install)
  -j, --jobs N      Parallel build jobs
EOF
            exit 0 ;;
        *) echo "Unknown argument: $1" >&2; exit 2 ;;
    esac
done

if [[ "$clean" -eq 1 ]]; then
    rm -rf -- "$build"
fi

args=(
    -S "$root"
    -B "$build"
    -DCMAKE_BUILD_TYPE="$config"
    -DRIBBONLIB_BUILD_EXAMPLE=ON
    -DRIBBONLIB_BUILD_TESTS=$([[ "$tests" -eq 1 ]] && echo ON || echo OFF)
    -DRIBBONLIB_BUILD_SHARED=$([[ "$shared" -eq 1 ]] && echo ON || echo OFF)
    -DRIBBONLIB_ENABLE_INSTALL=$([[ "$install" -eq 1 ]] && echo ON || echo OFF)
)

for name in Qt5_DIR Qt6_DIR CMAKE_PREFIX_PATH; do
    value="${!name:-}"
    [[ -z "$value" ]] || args+=("-D${name}=${value}")
done

cmake "${args[@]}"
cmake --build "$build" --parallel "$jobs"

if [[ "$tests" -eq 1 ]]; then
    ctest --test-dir "$build" --output-on-failure
fi

if [[ "$install" -eq 1 ]]; then
    cmake --install "$build" --prefix "$install_prefix"
fi

if [[ "$run" -eq 1 ]]; then
    exe="$build/example/RibbonExample"
    [[ -x "$exe" ]] || { echo "Example not found: $exe" >&2; exit 1; }
    exec "$exe"
fi
