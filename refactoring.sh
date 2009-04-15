#!/bin/sh

# Script to do refactoring of duma.. (put all files in extra subfolders to
# make it somewhat more organized ;-)

# planned folder structure:
#
# include/              <-- headers goe in here
# include/duma/         <-- duma headers go in here
# include/dumadetours/  <-- dumadetours headers go in here
# src/                  <-- sources go in here
# src/duma/             <-- duma sources go in here
# src/dumadetours/      <-- dumadetours sources go in here
# test/                 <-- tests go in here
# win32/                <-- all those win32* folders goe in here
# 

# move all win32-* folders to win32/
mkdir win32
mv win32-* win32/
