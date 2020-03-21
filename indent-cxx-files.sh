#!/bin/bash
# Helpcovid file indent-cxx-files.sh -- see
# https://github.com/bstarynk/helpcovid/](helpcovid) and
# https://unix.stackexchange.com/a/122848/50557
for f in "$@"; do
    cp -a $f $f~%
    ${ASTYLE:-astyle} ${ASTYLEFLAGS:- -v -s2 --style=gnu} $f
done
