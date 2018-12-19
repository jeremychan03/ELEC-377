#!/bin/bash

printf "%5s %-9s %5s %s\n" "PID" "USER" "RSS" "COMMAND"

for p in `ls -vd /proc/[0-9]*`; do
    
    # Check that status file exists, if not skip this process
    if [ ! -f $p/status ]; then
        continue
    fi
    
    # Get the PID
    PID=${p/\/proc\//}
    
    # Get the user
    USER=`stat -c %U $p`
    
    # Get the RSS, 0 by default
    RSS=`grep "VmRSS" $p/status`
    if [ -n "$RSS" ]; then
        RSS=${RSS/'VmRSS:'/}
        RSS=${RSS/'kB'/}
    else
        RSS="0"
    fi
    
    # Get the command
    CMD="$(cat $p/cmdline | tr '\000' ' ')"
    
    # If not cmdline exists, print out process name in [square brackets]
    if [ -z "$CMD" ]; then
        CMD=`grep "Name" $p/status | tr -d '\t '`
	CMD=${CMD#"Name:"}
	CMD='['$CMD']'
    fi
    
    # Print out the PID, USER, RSS, COMMAND
    printf "%5s %-9s %5s " $PID $USER $RSS
    echo $CMD 

done
