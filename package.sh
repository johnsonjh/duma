#!/bin/sh
echo usage: ./package.sh 2_4_12
export CVSROOT=":pserver:anonymous@duma.cvs.sourceforge.net:/cvsroot/duma"
rm -Rf duma duma_$1 duma_$1.zip duma_$1.tar.gz
cvs co -r VERSION_$1 duma
date >duma/VERSION
echo DUMA $1 >>duma/VERSION
echo "Actual version available from http://duma.sourceforge.net/" >>duma/VERSION
mv duma duma_$1
zip -r duma_$1.zip duma_$1
tar zcf duma_$1.tar.gz duma_$1
rm -Rf duma_$1
