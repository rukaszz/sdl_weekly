#!/usr/bin/env bash
set -o errexit  # set -e
set -o nounset  # set -u
set -o pipefail

# cmake .
# make
# ./build/sdl_week17

# get parent dir name
parent_dir=$(basename "$PWD")

cmake -G Ninja -S . -B build
ninja -C build -j8
./build/${parent_dir}
