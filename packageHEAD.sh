#!/bin/sh
echo usage: ./packageHEAD.sh
export CVSROOT=":pserver:anonymous@duma.cvs.sourceforge.net:/cvsroot/duma"
VER=`date -u +%Y%m%d_%H%M%S`
rm -Rf duma duma_$VER duma_$VER.zip duma_$VER.tar.gz
cvs co duma
date >duma/VERSION
echo DUMA HEAD >>duma/VERSION
echo "Actual version available from http://duma.sourceforge.net/" >>duma/VERSION
mv duma duma_$VER
zip -9 -r duma_$VER.zip duma_$VER
tar zcf duma_$VER.tar.gz duma_$VER
rm -Rf duma_$VER
