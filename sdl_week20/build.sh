#!/usr/bin/env bash
set -o errexit  # set -e
set -o nounset  # set -u
set -o pipefail

# cmake .
# make
# ./build/sdl_week17

cmake -G Ninja -S . -B build
ninja -C build -j8
./build/sdl_week20
