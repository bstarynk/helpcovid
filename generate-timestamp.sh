#!/bin/bash
printf "// generated file %s -- DONT EDIT\n" $1
date +"const char hcv_timestamp[]=\"%c\";%nconst unsigned long hcv_timelong=%sL;%n"
printf "const char hcv_topdirectory[]=\"%s\";\n" $(realpath $(pwd))

if git status|grep -q 'nothing to commit' ; then
    endgitid='";'
else
    endgitid='+";'
fi
(echo -n 'const char hcv_gitid[]="'; 
 git log --format=oneline -q -1 | cut '-d '  -f1 | tr -d '\n';
     echo $endgitid)  

(echo -n 'const char hcv_lastgittag[]="'; (git describe --abbrev=0 --all || echo '*notag*') | tr -d '\n\r\f\"\\\\'; echo '";')

(echo -n 'const char hcv_lastgitcommit[]="' ; \
 git log --format=oneline --abbrev=12 --abbrev-commit -q  \
     | head -1 | tr -d '\n\r\f\"\\\\' ; \
 echo '";') 

git archive -o /tmp/helpcovid-$$.tar.gz HEAD 
trap "/bin/rm /tmp/helpcovid-$$.tar.gz" EXIT INT 

cp -va /tmp/helpcovid-$$.tar.gz $HOME/tmp/helpcovid.tar.gz >& /dev/stderr

(echo -n 'const char hcv_md5sum[]="' ; cat $(tar tf /tmp/helpcovid-$$.tar.gz | grep -v '/$') | md5sum | tr -d '\n -'  ;  echo '";')

(echo  'const char*const hcv_files[]= {' ; tar tf /tmp/helpcovid-$$.tar.gz | grep -v '/$' | tr -s " \n"  | sed 's/^\(.*\)$/ "\1\",/';  echo ' (const char*)0} ;')

(echo  'const char*const hcv_subdirectories[]= {' ; tar tf /tmp/helpcovid-$$.tar.gz | grep  '/$' | tr -s " \n"  | sed 's/^\(.*\)$/ "\1\",/';  echo ' (const char*)0} ;')

printf "const char hcv_makefile[]=\"%s\";\n"   $(realpath Makefile)
