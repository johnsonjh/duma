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

# Set the environment to load eletric fence.
(\
export LD_PRELOAD=libduma.so.0.0;\
exec $*;\
)
