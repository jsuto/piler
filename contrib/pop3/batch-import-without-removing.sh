#!/bin/bash

counter_file="pop3-position"
COUNTER=0
STEPS=10

export PATH=$PATH:/usr/bin:/usr/local/bin

if [ ! -f $counter_file ]; then COUNTER=1; else COUNTER=`cat $counter_file`; fi


pilerimport -K pop3.youromain.com -u username -p password -s `cat $counter_file` -b $STEPS


COUNTER=`expr $COUNTER + $STEPS`

printf "%d" "$COUNTER" > $counter_file
