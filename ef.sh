#!/bin/bash

#
# Simple script to run electric fence on stuff.  (needs libefence.so).
#
# $Id$
#

# Usage check.
if [ "$1" = "" ]; then
	echo "$0: Usage: ef [executable] [arguments]."
	echo "	Runs the executable under the Electric Fence malloc debugger."
	exit
fi

# Set the environment to load eletric fence.
(\
export LD_PRELOAD=libefence.so.0.0;\
exec $*;\
)
