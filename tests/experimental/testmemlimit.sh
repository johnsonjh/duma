#!/usr/bin/env sh
set -e > /dev/null 2>&1
export DUMA_MAX_ALLOC=100
export DUMA_SHOW_ALLOC=1
./duma.sh ./testmemlimit_so
