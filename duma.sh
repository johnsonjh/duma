#!/bin/bash

#
# Simple script to run DUMA on stuff.  (needs libduma.so).
#
# $Id$
#

# Usage check.
if [ "$1" = "" ]; then
	echo "$0: Usage: duma [executable] [arguments]."
	echo "	Runs the executable under the DUMA malloc debugger."
	exit
fi

# Set the environment to (pre)load duma library.
(export LD_PRELOAD=libduma.so.0.0.0; export DYLD_INSERT_LIBRARIES=libduma.dylib; export DYLD_FORCE_FLAT_NAMESPACE=1; exec $*)

