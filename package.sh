#!/bin/sh
export CVSROOT=":pserver:anonymous@duma.cvs.sourceforge.net:/cvsroot/duma"

#translate system user to sourceforge username
SYS_USER=`whoami`
SF_USER=
if (test "$SYS_USER" = "ayguen" ); then
  SF_USER=h_ayguen
fi

if (test "$1" = ""); then
  echo "usage: $0 2_5_13|HEAD [upload] [htdocs]"
  echo "  '2_5_14|HEAD'  is the BRANCH to check out"
  echo "  'upload'       uploads all necessary files to sf.net for a file release"
  echo "  'htdocs'       uploads html documents to sf.net"
  exit 0;
elif (test "$1" = "HEAD"); then
  VER=`date -u +%Y%m%d_%H%M%S`
  REV=HEAD
else
  VER=$1
  REV=VERSION_$1
fi

rm -Rf duma duma_$VER duma_$VER.zip duma_$VER.tar.gz
cvs co -r $REV duma
date -u -R >duma/VERSION
echo DUMA $VER $REV >>duma/VERSION
echo "Actual version available from http://duma.sourceforge.net/" >>duma/VERSION
mv duma duma_$VER
zip -9 -r duma_$VER.zip duma_$VER -x 'CVS*'
tar zcf duma_$VER.tar.gz --exclude 'CVS*' duma_$VER

if (test "$SF_USER" = "" ); then
  echo invalid SF_USER $SF_USER
fi

if ((test "$2" = "upload") || (test "$3" = "upload")); then
  echo uploading file duma_$VER.zip
  scp duma_$VER.zip    $SF_USER@frs.sourceforge.net:uploads
  echo uploading file duma_$VER.tar.gz
  scp duma_$VER.tar.gz $SF_USER@frs.sourceforge.net:uploads
fi

if ((test "$2" = "htdocs") || (test "$3" = "htdocs")); then
  echo updating sourceforge htdocs
  scp duma_$VER/README.txt $SF_USER,duma@frs.sourceforge.net:/home/groups/d/du/duma/htdocs/README.txt
  scp duma_$VER/INSTALL    $SF_USER,duma@frs.sourceforge.net:/home/groups/d/du/duma/htdocs/INSTALL
  scp duma_$VER/CHANGELOG  $SF_USER,duma@frs.sourceforge.net:/home/groups/d/du/duma/htdocs/CHANGELOG
  scp duma_$VER/TODO       $SF_USER,duma@frs.sourceforge.net:/home/groups/d/du/duma/htdocs/TODO
fi

rm -Rf duma_$VER


