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
zip -9 -r duma_$VER.zip duma_$VER
tar zcf duma_$VER.tar.gz duma_$VER

if (test "$SF_USER" = "" ); then
  echo invalid SF_USER $SF_USER
fi

cat >ftp_upload.pl <<EOF
#!/usr/bin/perl
use Net::FTP;

if ( @ARGV <= 0 ) {
  print "no file argument given!\n";
  exit 10;
}

my \$file = "@ARGV[0]";
my \$host = 'upload.sourceforge.net';
my \$user = 'anonymous';
my \$password = 'anonymous';
my \$dir = '/incoming';

# Neues Net::FTP-Objekt
my \$ftp = Net::FTP->new(\$host,
                        Timeout => 360,
                        Debug   => 0
                       );
unless (defined \$ftp) {
	print "\$@\n";
	die "Can't create Net::FTP-Object\n";
}

\$ftp->login(\$user,\$password) || die "Can't login \$!";
\$ftp->cwd(\$dir) || die "Can't change working directory \$dir \$!";
\$ftp->type(I);
\$remote_file_name = \$ftp->put(\$file) || die "Can't put \$file \$!";
\$ftp->quit();
EOF

if ((test "$2" = "upload") || (test "$3" = "upload")); then
  chmod a+x ftp_upload.pl
  echo uploading file duma_$VER.zip
  ./ftp_upload.pl duma_$VER.zip
  echo uploading file duma_$VER.tar.gz
  ./ftp_upload.pl duma_$VER.tar.gz
fi

if ((test "$2" = "htdocs") || (test "$3" = "htdocs")); then
  echo updating sourceforge htdocs
  scp duma_$VER/README.txt $SF_USER@shell.sourceforge.net:/home/groups/d/du/duma/htdocs/README.txt
  scp duma_$VER/INSTALL    $SF_USER@shell.sourceforge.net:/home/groups/d/du/duma/htdocs/INSTALL
  scp duma_$VER/CHANGELOG  $SF_USER@shell.sourceforge.net:/home/groups/d/du/duma/htdocs/CHANGELOG
  scp duma_$VER/TODO       $SF_USER@shell.sourceforge.net:/home/groups/d/du/duma/htdocs/TODO
fi

rm -f ./ftp_upload.pl
rm -Rf duma_$VER
