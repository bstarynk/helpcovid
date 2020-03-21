#!/bin/sh
# helpcovid file generate-gitid.sh - see [https://github.com/bstarynk/helpcovid/](helpcovid)
###
# it just emits a string with the full git commit id, appending + if
# the git status is not clean.
if git status|grep -q 'nothing to commit' ; then
    endgitid=''
else
    endgitid='+'
fi
(git log --format=oneline -q -1 | cut '-d '  -f1 | tr -d '\n';
     echo $endgitid)  
