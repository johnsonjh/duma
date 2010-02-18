#!/usr/bin/env sh

# Simple script to run DUMA on stuff.  
# (Requires libduma.so.0.0 insalled).

# Usage.
if [ "$#" -lt 1 ]; then
	printf '%s\n' "$0:	Usage: duma <executable> [optional arguments]."
	printf '%s\n' "		Runs the executable under the DUMA malloc debugger."
	exit 1
fi

# Set the environment to preload DUMA library.
export LD_PRELOAD=libduma.so.0.0.0
export DYLD_INSERT_LIBRARIES=libduma.dylib
export DYLD_FORCE_FLAT_NAMESPACE=1
exec "$@"
