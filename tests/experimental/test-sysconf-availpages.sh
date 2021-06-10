#!/usr/bin/env sh
set -e > /dev/null 2>&1
export DUMA_DISABLE_BANNER=1
gcc test-sysconf-availpages.c -o test-sysconf-availpages

printf '%s\n' "Running tests ..."
./test-sysconf-availpages 64000
./test-sysconf-availpages 128000
./test-sysconf-availpages 256000
