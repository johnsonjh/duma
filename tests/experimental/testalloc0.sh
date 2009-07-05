#!/bin/bash
export DUMA_DISABLE_BANNER=1
g++ testalloc0.cpp -o testalloc0

echo tests without duma
./testalloc0 0
./testalloc0 1
./testalloc0 2

echo tests with duma
export LD_PRELOAD=./libduma.so.0.0.0
export DYLD_INSERT_LIBRARIES=./libduma.dylib
export DYLD_FORCE_FLAT_NAMESPACE=1
./testalloc0 0
./testalloc0 1
./testalloc0 2

