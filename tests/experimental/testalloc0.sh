#!/usr/bin/env sh
set -e > /dev/null 2>&1
export DUMA_DISABLE_BANNER=1
g++ testalloc0.cpp -o testalloc0

printf '%s\n' "Testing without DUMA"
./testalloc0 0
./testalloc0 1
./testalloc0 2

printf '%s\n' "Testing with DUMA"
export LD_PRELOAD="./libduma.so.0.0.0"
export DYLD_INSERT_LIBRARIES="./libduma.dylib"
export DYLD_FORCE_FLAT_NAMESPACE=1
./testalloc0 0
./testalloc0 1
./testalloc0 2
