#!/bin/bash
# project https://github.com/bstarynk/helpcovid
# in directory examples/deploy_b-star-y/
# shell script restart_helpcovid_b-star-y.sh
# to be run every five minutes from crontab
# to be installed as $HOME/bin/restart_helpcovid_b-star-y.sh
# for user helpcovidu
# see also the crontab file in same directory
HELPCOVIDRUN_PIDFILE=/tmp/helpcovidrun.pid
HELPCOVIDRUN_TAG=$(/usr/bin/basename $0)
HELPCOVIDRUN_TIME=$(/usr/bin/date +"%c")
HELPCOVIDRUN_SRCDIR=$HOME/helpcovid/
HELPCOVIDRUN_PWD=$(pwd)
HELPCOVIDRUN_HOST=$(/usr/bin/hostname --fqdn)
HELPCOVIDRUN_THREADS=2
HELPCOVIDRUN_FLAGS="-D"

# left empty on purpose:
HELPCOVIDRUN_OLDPID=

##### function to log as notice
helpcovidrun_log_notice() {
    /usr/bin/logger --id=$$ -tag="$HELPCOVIDRUN_TAG" --priority=user.notice "$@"
}

##### function to log as information
helpcovidrun_log_info() {
    /usr/bin/logger --id=$$ -tag="$HELPCOVIDRUN_TAG" --priority=user.info "$@"
}


##### function to log as warning
helpcovidrun_log_warning() {
    /usr/bin/logger --id=$$ -tag="$HELPCOVIDRUN_TAG" --priority=user.warning "$@"
}


#### logging start
helpcovidrun_log_notice starting $0 at "$HELPCOVIDRUN_TIME" inside "$HELPCOVIDRUN_PWD" on "$HELPCOVIDRUN_HOST"

#### sourcing site-specific parameters
if [ -f "$HOME/helpcovidrun_settings" ]; then    
    helpcovidrun_log_info sourcing  "$HOME/helpcovidrun_settings"
    source "$HOME/helpcovidrun_settings"
fi


### check validity of source directory
if [ ! -d "$HELPCOVIDRUN_SRCDIR" ]; then
    helpcovidrun_log_warning invalid source directory "$HELPCOVIDRUN_SRCDIR"
    exit 1
fi

if [ ! -f "$HELPCOVIDRUN_SRCDIR/hcv_header.hh" ]; then
    helpcovidrun_log_warning source directory "$HELPCOVIDRUN_SRCDIR" without hcv_header.hh
    exit 1
fi


if [ ! -f "$HELPCOVIDRUN_SRCDIR/_make.out" ]; then
    /usr/bin/mv -v --backup "$HELPCOVIDRUN_SRCDIR/_make.out" "/tmp/helpcovid_make.out~"
fi

#### noticing the pid file of previously running helpcovid process
if [ -f "$HELPCOVIDRUN_PIDFILE" ]; then
    HELPCOVIDRUN_OLDPID=$(/bin/cat HELPCOVIDRUN_PIDFILE)
    helpcovidrun_log_info noticing oldpid "$HELPCOVIDRUN_OLDPID" running $(/usr/bin/readlink "/proc/$HELPCOVIDRUN_OLDPID/exe")
fi

(cd  $HELPCOVIDRUN_SRCDIR ; git pull)
if [ $? -ne 0 ]; then
    helpcovidrun_log_warning git pull failed inside "$HELPCOVIDRUN_SRCDIR" with exit code $?
    exit 1
else
     helpcovidrun_log_info git pull succeeded inside  "$HELPCOVIDRUN_SRCDIR" 
fi

time /usr/bin/make -C $HELPCOVIDRUN_SRCDIR < /dev/null >  "$HELPCOVIDRUN_SRCDIR/_make.out" 2>&1
if [ $? -ne 0 ]; then
    helpcovidrun_log_warning make failed with exit code $?
    exit 2
else
    /usr/bin/date +"### end make in  $HELPCOVIDRUN_SRCDIR at %c%n" >>  "$HELPCOVIDRUN_SRCDIR/_make.out"
     helpcovidrun_log_info make succeeded inside "$HELPCOVIDRUN_SRCDIR"
    sleep 1
fi

### killing the previously running helpcovid process
if [ -n "$HELPCOVIDRUN_OLDPID" ]; then
   helpcovidrun_log_notice killing oldpid "$HELPCOVIDRUN_OLDPID" 
    kill -TERM $HELPCOVIDRUN_OLDPID
    /bin/mv -f "$HELPCOVIDRUN_PIDFILE" "$HELPCOVIDRUN_PIDFILE~"
    sleep 1
fi

if [ ! -x "$HELPCOVIDRUN_SRCDIR/helpcovid" ]; then
    helpcovidrun_log_warning  "$HELPCOVIDRUN_SRCDIR/helpcovid" is not executable
    exit 1
fi

nohup $HELPCOVIDRUN_SRCDIR/helpcovid $HELPCOVIDRUN_FLAGS --threads=$HELPCOVIDRUN_THREADS --write-pid=$HELPCOVIDRUN_PIDFILE < /dev/null &
